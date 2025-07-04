#include "GameFlow/NS_LobbyMode.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFlow/NS_GameInstance.h"
#include "GameFramework/GameState.h"
#include "NS_LobbyController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

ANS_LobbyMode::ANS_LobbyMode()
{
	DefaultPawnClass = nullptr;
}

void ANS_LobbyMode::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinInProgress = true;

	Sessions->CreateSession(0, NAME_GameSession, SessionSettings);

	UE_LOG(LogTemp, Warning, TEXT("Dedicated Server에서 CreateSession 호출됨"));
}

void ANS_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("UNS_GameInstance를 찾을 수 없습니다!"));
		return;
	}

	int32 PlayerIndex = GetGameState<AGameState>()->PlayerArray.Num() - 1;

	if (!PawnClassesToSpawn.IsValidIndex(PlayerIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("PawnClassesToSpawn 배열에 PlayerIndex %d에 해당하는 클래스가 없습니다. 배열 크기를 확인해주세요!"), PlayerIndex);
		return;
	}

	AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
	if (!StartSpot)
	{
		UE_LOG(LogTemp, Error, TEXT("Spawn point not found for index %d."), PlayerIndex);
		return;
	}

	FTransform SpawnTransform = StartSpot->GetActorTransform();

	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClassesToSpawn[PlayerIndex], SpawnTransform);
	if (SpawnedPawn)
	{
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s from LobbyMode array"), *SpawnedPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed."));
	}

	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
	{
		PS->SetPlayerIndex(PlayerIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState cast to ANS_PlayerState failed."));
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(*It))
		{
			if (UNS_GameInstance* GameInst = Cast<UNS_GameInstance>(GetGameInstance()))
			{
				if (GameInst->ReadyUIInstance)
				{
					GameInst->ReadyUIInstance->UpdatePlayerStatusList();
				}
			}
		}
	}
}

void ANS_LobbyMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* PlayerController = Cast<APlayerController>(Exiting);
	if (!PlayerController) return;

	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PlayerController->PlayerState))
	{
		UE_LOG(LogTemp, Log, TEXT("Player %s with index %d has left the lobby."), *PS->GetPlayerName(), PS->PlayerIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast PlayerState to ANS_PlayerState during logout."));
	}

	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (GI->ReadyUIInstance)
		{
			GI->ReadyUIInstance->UpdatePlayerStatusList();
		}
	}

	// Flask 서버에 플레이어 로그아웃 알림
	NotifyPlayerLogout();
}

AActor* ANS_LobbyMode::FindSpawnPointByIndex(int32 Index)
{
	int32 CurrentIndex = 0;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (CurrentIndex == Index)
		{
			return *It;
		}
		++CurrentIndex;
	}
	return nullptr;
}

void ANS_LobbyMode::CheckAllPlayersReady()
{
	const AGameStateBase* GS = GetGameState<AGameStateBase>();
	if (!GS) return;

	//// 플레이어 수 체크
	//if (GS->PlayerArray.Num() < 2)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("플레이어 수 부족. 현재 인원: %d"), GS->PlayerArray.Num());
	//	return;
	//}

	// 전원 레디 확인
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (ANS_PlayerState* MyPS = Cast<ANS_PlayerState>(PS))
		{
			if (!MyPS->GetIsReady())
			{
				UE_LOG(LogTemp, Warning, TEXT("아직 준비 안된 플레이어 있음"));
				return;
			}
		}
	}

	// 모든 플레이어에게 로딩 스크린 표시
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(PC))
			{
				// Client_ShowWait 제거 - 불필요한 호출 방지
				LC->Client_ShowLoadingScreen(); // 로딩 스크린 표시
			}
		}
	}

	// 딜레이 최소화 - 대기 화면 방지
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerTravel 즉시 실행: MainWorld 이동"));
		const FString LevelPath = TEXT("/Game/Maps/MainWorld");
		const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");
		GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
	});
}

// Flask 서버에 플레이어 로그인 알림 (LobbyMode)
void ANS_LobbyMode::NotifyPlayerLogin()
{
	if (!HasAuthority()) return;

	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (GI->MyServerPort > 0)
		{
			// HTTP 요청으로 플레이어 로그인 알림
			FHttpModule& HttpModule = FHttpModule::Get();
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

			Request->SetURL(TEXT("http://121.163.249.108:5000/player_login"));
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

			// JSON 데이터 생성
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			JsonObject->SetNumberField(TEXT("port"), GI->MyServerPort);

			FString OutputString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			Request->SetContentAsString(OutputString);

			Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
			{
				if (bWasSuccessful && Response.IsValid())
				{
					UE_LOG(LogTemp, Log, TEXT("[LobbyMode-NotifyPlayerLogin] 플레이어 로그인 알림 성공"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[LobbyMode-NotifyPlayerLogin] 플레이어 로그인 알림 실패"));
				}
			});

			Request->ProcessRequest();
		}
	}
}

// Flask 서버에 플레이어 로그아웃 알림 (LobbyMode)
void ANS_LobbyMode::NotifyPlayerLogout()
{
	if (!HasAuthority()) return;

	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (GI->MyServerPort > 0)
		{
			// HTTP 요청으로 플레이어 로그아웃 알림
			FHttpModule& HttpModule = FHttpModule::Get();
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

			Request->SetURL(TEXT("http://121.163.249.108:5000/player_logout"));
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

			// JSON 데이터 생성
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			JsonObject->SetNumberField(TEXT("port"), GI->MyServerPort);

			FString OutputString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			Request->SetContentAsString(OutputString);

			Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
			{
				if (bWasSuccessful && Response.IsValid())
				{
					UE_LOG(LogTemp, Log, TEXT("[LobbyMode-NotifyPlayerLogout] 플레이어 로그아웃 알림 성공"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[LobbyMode-NotifyPlayerLogout] 플레이어 로그아웃 알림 실패"));
				}
			});

			Request->ProcessRequest();
		}
	}
}