#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/NS_LoadingScreen.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "UI/NS_UIManager.h"

UNS_GameInstance::UNS_GameInstance()
{
	static ConstructorHelpers::FClassFinder<UNS_UIManager> BP_UIManager(TEXT("/Game/UI/Blueprints/BP_NS_UIManager.BP_NS_UIManager_C"));
	if (BP_UIManager.Succeeded())
	{
		UIManagerClass = BP_UIManager.Class;
		UE_LOG(LogTemp, Warning, TEXT("BP_NS_UIManager 블루프린트 로드 성공"));
	}
	else
	{
		// 블루프린트를 찾지 못한 경우 C++ 클래스 직접 사용
		UIManagerClass = UNS_UIManager::StaticClass();
		UE_LOG(LogTemp, Warning, TEXT("BP_NS_UIManager 블루프린트를 찾지 못해 C++ 클래스 사용"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> BP_LoadingWait(TEXT("/Game/UI/Blueprints/BP_Wait.BP_Wait_C"));
	if (BP_LoadingWait.Succeeded())
	{
		WaitClass = BP_LoadingWait.Class;
	}

	// GameMode 강제 참조로 패키징 포함 유도
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeMulti(TEXT("/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C"));
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeSingle(TEXT("/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C"));
}

void UNS_GameInstance::Init()
{
	Super::Init();

	if (UIManagerClass)
	{
		NS_UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);
		NS_UIManager->InitUi(GetWorld());
	}

	// 레벨 로드 완료 감지를 위한 타이머 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(LevelLoadCheckTimer,
			FTimerDelegate::CreateUObject(this, &UNS_GameInstance::CheckForLevelLoadComplete),
			2.0f, true); // 2초마다 체크
	}

	// Dedicated 서버가 실행될 경우, 커맨드라인에서 포트 추출
	if (IsRunningDedicatedServer())
	{
		FString PortStr;
		if (FParse::Value(FCommandLine::Get(), TEXT("PORT="), PortStr))
		{
			MyServerPort = FCString::Atoi(*PortStr);
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 커맨드라인에서 포트 추출: %d"), MyServerPort);
			
			
			GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &UNS_GameInstance::SendHeartbeat, 10.0f, true);
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 데디케이트 서버 하트비트를 시작합니다."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 커맨드라인에서 포트 추출 실패. FCommandLine: %s"), FCommandLine::Get());
		}
	}
}



void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}

void UNS_GameInstance::CreateDedicatedSessionViaHTTP(FName SessionName, int32 MaxPlayers)
{
	UE_LOG(LogTemp, Log, TEXT("[CreateDedicatedSessionViaHTTP] Sending HTTP POST: name=%s, max_players=%d"),
		*SessionName.ToString(), MaxPlayers);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/create_session"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", SessionName.ToString());
	JsonObject->SetNumberField("max_players", MaxPlayers);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnCreateSessionResponse);
	Request->ProcessRequest();
}

void UNS_GameInstance::OnCreateSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] HTTP 요청 실패"));
		return;
	}

	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] HTTP Response: %s"), *ResponseString);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] JSON 파싱 실패: %s"), *ResponseString);
		return;
	}

	FString Ip = JsonObject->GetStringField(TEXT("ip"));
	int32 Port = JsonObject->GetIntegerField(TEXT("port"));
	FString Address = FString::Printf(TEXT("%s:%d"), *Ip, Port);
	MyServerPort = Port;

	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] 접속 주소: %s"), *Address);

	// 10초마다 heartbeat 보낸다.
	//GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &UNS_GameInstance::SendHeartbeat, 10.0f, true);

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] PlayerController 획득 실패"));
		}
	}
}

void UNS_GameInstance::SendHeartbeat()
{
	if (MyServerPort <= 0)
		return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/heartbeat"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetNumberField("port", MyServerPort);

	FString Body;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
	Request->SetContentAsString(Body);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (!bSuccess || !Res.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Heartbeat] Failed to reach server."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Heartbeat] Response: %s"), *Res->GetContentAsString());
		}
	});

	Request->ProcessRequest();
}


void UNS_GameInstance::RequestUpdateSessionStatus(int32 Port, FString Status)
{
	UE_LOG(LogTemp, Log, TEXT("[RequestUpdateSessionStatus] Sending HTTP POST to update session status. Port: %d, Status: %s"), Port, *Status);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/update_session_status")); 
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField("port", Port);
	JsonObject->SetStringField("status", Status); 

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnUpdateSessionStatusResponse);
	Request->ProcessRequest();
}

void UNS_GameInstance::OnUpdateSessionStatusResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnUpdateSessionStatusResponse] HTTP 요청 실패"));
		return;
	}

	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnUpdateSessionStatusResponse] HTTP Response: %s"), *ResponseString);

}


void UNS_GameInstance::RequestSessionListFromServer()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/session_list"));
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnReceiveSessionList);
	Request->ProcessRequest();
}

void UNS_GameInstance::OnReceiveSessionList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] HTTP 요청 실패"));
		return;
	}

	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] HTTP Response: %s"), *ResponseString);

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] JSON 배열 파싱 실패"));
		return;
	}

	TArray<TSharedPtr<FJsonObject>> ParsedSessions;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		TSharedPtr<FJsonObject> SessionObj = Value->AsObject();
		if (SessionObj.IsValid())
		{
			ParsedSessions.Add(SessionObj);
		}
	}

	OnSessionListReceived.Broadcast(ParsedSessions); 
}

void UNS_GameInstance::SetCurrentSaveSlot(FString SlotNameInfo)
{
	CurrentSaveSlotName = SlotNameInfo;
}

void UNS_GameInstance::ShowReadyUI()
{
	UE_LOG(LogTemp, Warning, TEXT(" ShowReadyUI() 진입"));

	if (!ReadyUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT(" ReadyUIClass is NULL! 위젯 생성 불가"));
		return;
	}

	if (!ReadyUIInstance)
	{
		ReadyUIInstance = CreateWidget<UNS_ReadyUI>(this, ReadyUIClass);
		UE_LOG(LogTemp, Warning, TEXT("ReadyUIInstance 생성 완료: %s"), *GetNameSafe(ReadyUIInstance));
	}

	if (ReadyUIInstance && !ReadyUIInstance->IsInViewport())
	{
		ReadyUIInstance->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT(" ReadyUIInstance AddToViewport 완료"));
	}
}


void UNS_GameInstance::HideReadyUI()
{
	if (ReadyUIInstance && ReadyUIInstance->IsInViewport())
	{
		ReadyUIInstance->RemoveFromParent();
	}
}

void UNS_GameInstance::ShowWait()
{
	if (!WaitClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	if (!WaitWidget)
	{
		WaitWidget = CreateWidget<UUserWidget>(PC, WaitClass);
	}

	if (WaitWidget && !WaitWidget->IsInViewport())
	{
		WaitWidget->AddToViewport();
	}
}

void UNS_GameInstance::HideWait()
{
	if (WaitWidget && WaitWidget->IsInViewport())
	{
		WaitWidget->RemoveFromParent();
	}
}

void UNS_GameInstance::DestroyCurrentSession()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession(NAME_GameSession);
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 세션 파기 요청됨"));
	}
}

UNS_BaseMainMenu* UNS_GameInstance::GetMainMenu()
{
	// MainMenu가 null이면 생성
	if (!MainMenu && MainMenuClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PC = World->GetFirstPlayerController();
			if (PC)
			{
				MainMenu = CreateWidget<UNS_BaseMainMenu>(PC, MainMenuClass);
				UE_LOG(LogTemp, Log, TEXT("MainMenu created in GetMainMenu()"));
			}
		}
	}

	return MainMenu;
}

void UNS_GameInstance::CheckForLevelLoadComplete()
{
	UWorld* World = GetWorld();
	if (!World || bFrameRateCheckStarted) return;

	// NS_UIManager 유효성 체크
	if (!NS_UIManager || !IsValid(NS_UIManager))
	{
		UE_LOG(LogTemp, Error, TEXT("CheckForLevelLoadComplete: UIManager가 유효하지 않습니다"));
		return;
	}

	// 현재 레벨이 MainWorld인지 확인 (게임 레벨)
	FString CurrentLevelName = World->GetMapName();
	if (CurrentLevelName.Contains(TEXT("MainWorld")) && World->HasBegunPlay())
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckForLevelLoadComplete: MainWorld 레벨 로드 완료 감지"));

		// 타이머 정리
		World->GetTimerManager().ClearTimer(LevelLoadCheckTimer);
		bFrameRateCheckStarted = true;

		// 프레임률 체크 시작 (안전하게)
		StartPostLevelLoadFrameRateCheck();
	}
}

void UNS_GameInstance::StartPostLevelLoadFrameRateCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("StartPostLevelLoadFrameRateCheck: 레벨 로드 완료 후 프레임률 체크 시작"));

	// UIManager 안전성 체크
	if (!NS_UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("StartPostLevelLoadFrameRateCheck: UIManager가 null입니다"));
		return;
	}

	if (!IsValid(NS_UIManager))
	{
		UE_LOG(LogTemp, Error, TEXT("StartPostLevelLoadFrameRateCheck: UIManager가 유효하지 않습니다"));
		NS_UIManager = nullptr;
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("StartPostLevelLoadFrameRateCheck: World가 null입니다"));
		return;
	}

	// 영구 로딩 스크린이 있으면 프레임률 체크 모드로 전환
	if (PersistentLoadingScreen)
	{
		// 현재 진행률 확인
		float CurrentProgress = PersistentLoadingScreen->GetCurrentProgress();
		UE_LOG(LogTemp, Warning, TEXT("영구 로딩 스크린 연속 모드 전환 - 현재 진행률: %.1f%%"), CurrentProgress * 100.0f);

		// 로딩이 완료되지 않았다면 다시 시작
		if (!PersistentLoadingScreen->IsLoadingComplete())
		{
			PersistentLoadingScreen->StartLoading();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("영구 로딩 스크린이 없습니다"));
	}
}

void UNS_GameInstance::CreatePersistentLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 함수 호출됨"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: World가 null입니다"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: World 확인 완료"));

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: PlayerController가 null입니다"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: PlayerController 확인 완료"));

	if (!NS_UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: UIManager가 null입니다"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: UIManager 확인 완료"));

	TSubclassOf<UNS_LoadingScreen> LoadingScreenClass = NS_UIManager->GetLoadingScreenClass();
	if (!LoadingScreenClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: LoadingScreenClass가 null입니다"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: LoadingScreenClass 확인 완료"));

	// 기존 영구 로딩 스크린이 있으면 제거
	if (PersistentLoadingScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 기존 로딩 스크린 제거"));
		PersistentLoadingScreen->RemoveFromParent();
		PersistentLoadingScreen = nullptr;
	}

	// 새로운 영구 로딩 스크린 생성
	UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 위젯 생성 시도"));
	PersistentLoadingScreen = CreateWidget<UNS_LoadingScreen>(PC, LoadingScreenClass);
	if (PersistentLoadingScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 위젯 생성 성공"));

		// 최상위 Z-Order로 추가하여 레벨 전환 시에도 유지
		PersistentLoadingScreen->AddToViewport(10000);

		// 강제로 보이게 설정
		PersistentLoadingScreen->SetVisibility(ESlateVisibility::Visible);

		UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 뷰포트에 추가 완료 - Z-Order: 10000"));

		// GameInstance가 소유하므로 AddToRoot 불필요 (UPROPERTY로 보호됨)

		// 로딩 스크린 초기화 및 시작
		PersistentLoadingScreen->InitializeLoadingScreen();
		PersistentLoadingScreen->StartLoading();
		UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 로딩 시작"));

		// 입력 모드 설정
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;

		UE_LOG(LogTemp, Warning, TEXT("영구 로딩 스크린 생성 완료 - 레벨 전환에도 유지됨"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("영구 로딩 스크린 생성 실패 - CreateWidget 반환값이 null"));
	}
}

void UNS_GameInstance::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 정리 시작"));

	// 영구 로딩 스크린 안전하게 정리
	if (PersistentLoadingScreen)
	{
		if (PersistentLoadingScreen->IsInViewport())
		{
			PersistentLoadingScreen->RemoveFromParent();
		}
		PersistentLoadingScreen = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 영구 로딩 스크린 정리 완료"));
	}

	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LevelLoadCheckTimer);
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 타이머 정리 완료"));
	}

	Super::Shutdown();
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 완료"));
}