#include "GameFlow/NS_LobbyMode.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "NS_GameInstance.h"
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

ANS_LobbyMode::ANS_LobbyMode()
{
	DefaultPawnClass = nullptr; // 자동 스폰 방지
}

void ANS_LobbyMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ANS_LobbyMode::BeginPlay 진입"));
	UE_LOG(LogTemp, Warning, TEXT("LobbyCharacterClasses 수: %d"), LobbyCharacterClasses.Num());

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

	int32 PlayerIndex = GetGameState<AGameState>()->PlayerArray.Num() - 1;

	if (!LobbyCharacterClasses.IsValidIndex(PlayerIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyCharacterClasses[%d] 존재하지 않음! 배열 크기: %d"), PlayerIndex, LobbyCharacterClasses.Num());
		return;
	}

	AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
	if (!StartSpot)
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for index %d"), PlayerIndex);
		return;
	}

	FTransform SpawnTransform = StartSpot->GetActorTransform();
	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(LobbyCharacterClasses[PlayerIndex], SpawnTransform);

	if (SpawnedPawn)
	{
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s"), *SpawnedPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed."));
	}

	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
	{
		PS->SetPlayerIndex(PlayerIndex);
		PS->SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState cast to ANS_PlayerState failed."));
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(*It))
		{
			if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
			{
				if (GI->ReadyUIInstance)
				{
					GI->ReadyUIInstance->UpdatePlayerStatusList();
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

	// 전원 레디 완료됨 → 클라이언트에 ShowWait 전달 (UI만 띄우는 용도)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// ShowWait은 클라이언트에만 띄움 → 반드시 클라이언트 RPC로 분리
			if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(PC))
			{
				LC->Client_ShowWait();
			}
		}
	}

	// 안전한 ServerTravel → 1초 지연 후 람다로 실행
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerTravel 실행: MainWorld 이동"));
			const FString LevelPath = TEXT("/Game/Maps/MainWorld");
			const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");
			GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
		}, 1.0f, false);
	});
}

