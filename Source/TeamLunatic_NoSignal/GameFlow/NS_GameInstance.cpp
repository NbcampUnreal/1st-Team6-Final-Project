#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

UNS_GameInstance::UNS_GameInstance()
{
	// GameMode 강제 참조로 패키징 포함 유도
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeMulti(TEXT("/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C"));
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeSingle(TEXT("/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C"));
}

void UNS_GameInstance::Init()
{
	Super::Init();
	// Dedicated 서버가 실행될 경우, 커맨드라인에서 포트 추출
	if (IsRunningDedicatedServer())
	{
		FString PortStr;
		if (FParse::Value(FCommandLine::Get(), TEXT("PORT="), PortStr))
		{
			MyServerPort = FCString::Atoi(*PortStr);
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 커맨드라인에서 포트 추출: %d"), MyServerPort);
			
			
			GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &UNS_GameInstance::SendHeartbeat, 20.0f, true);
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 데디케이트 서버 하트비트를 시작합니다 (20초마다)"));
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
	Request->SetTimeout(10.0f); // 타임아웃 시간 증가 (기본 5초 → 10초)

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetNumberField("port", MyServerPort);

	// 현재 플레이어 수 추가
	int32 CurrentPlayerCount = 0;
	if (UWorld* World = GetWorld())
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			CurrentPlayerCount = GameMode->GetNumPlayers();
		}
	}
	Json->SetNumberField("current_players", CurrentPlayerCount);

	FString Body;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
	Request->SetContentAsString(Body);

	Request->OnProcessRequestComplete().BindLambda([CurrentPlayerCount](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (!bSuccess || !Res.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Heartbeat] Failed to reach server."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Heartbeat] Response: %s (Players: %d)"), *Res->GetContentAsString(), CurrentPlayerCount);
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
	UE_LOG(LogTemp, Log, TEXT("[RequestSessionListFromServer] 세션 리스트 요청 시작"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/session_list"));
	Request->SetVerb(TEXT("GET"));
	Request->SetTimeout(10.0f); // 타임아웃 시간 설정
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnReceiveSessionList);

	bool bRequestStarted = Request->ProcessRequest();
	UE_LOG(LogTemp, Log, TEXT("[RequestSessionListFromServer] HTTP 요청 시작됨: %s"), bRequestStarted ? TEXT("성공") : TEXT("실패"));
}

void UNS_GameInstance::OnReceiveSessionList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] HTTP 요청 실패 - bWasSuccessful: false"));
		return;
	}

	if (!Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] HTTP 응답이 유효하지 않음"));
		return;
	}

	int32 ResponseCode = Response->GetResponseCode();
	FString ResponseString = Response->GetContentAsString();

	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] HTTP 응답 코드: %d"), ResponseCode);
	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] HTTP 응답 내용: %s"), *ResponseString);

	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] HTTP 응답 코드 오류: %d"), ResponseCode);
		return;
	}

	// 빈 응답 체크
	if (ResponseString.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnReceiveSessionList] 빈 응답 받음"));
		// 빈 배열로 처리
		TArray<TSharedPtr<FJsonObject>> EmptySessionList;
		OnSessionListReceived.Broadcast(EmptySessionList);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] JSON 배열 파싱 실패. 응답 내용: %s"), *ResponseString);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] JSON 배열 파싱 성공. 세션 수: %d"), JsonArray.Num());

	TArray<TSharedPtr<FJsonObject>> ParsedSessions;
	for (int32 i = 0; i < JsonArray.Num(); i++)
	{
		const TSharedPtr<FJsonValue>& Value = JsonArray[i];
		TSharedPtr<FJsonObject> SessionObj = Value->AsObject();
		if (SessionObj.IsValid())
		{
			// 세션 정보 로깅
			FString SessionName = SessionObj->GetStringField(TEXT("name"));
			FString SessionIP = SessionObj->GetStringField(TEXT("ip"));
			int32 SessionPort = SessionObj->GetIntegerField(TEXT("port"));
			int32 CurrentPlayers = SessionObj->GetIntegerField(TEXT("current_players"));
			int32 MaxPlayers = SessionObj->GetIntegerField(TEXT("max_players"));

			UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] 세션 %d: %s (%s:%d) - %d/%d 플레이어"),
				i, *SessionName, *SessionIP, SessionPort, CurrentPlayers, MaxPlayers);

			ParsedSessions.Add(SessionObj);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[OnReceiveSessionList] 세션 %d: JSON 객체 파싱 실패"), i);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] 파싱된 세션 수: %d, 브로드캐스트 시작"), ParsedSessions.Num());
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