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
    DefaultPawnClass = nullptr; // 자동 생성 방지

}




void ANS_LobbyMode::BeginPlay()
{
    Super::BeginPlay();


    if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        for (const FString& Item : GameInstance->CharacterList)
        {
            UClass* LoadedClass = StaticLoadClass(APawn::StaticClass(), nullptr, *Item);
            if (LoadedClass)
            {
                PlayableCharacter.Add(LoadedClass);
                UE_LOG(LogTemp, Log, TEXT("Loaded character class: %s"), *Item);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load character blueprint class: %s"), *Item);
            }
        }

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to UNS_GameInstance"));
    }

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

    AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
    if (!StartSpot)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for index %d"), PlayerIndex);
        return;
    }

    FTransform SpawnTransform = StartSpot->GetActorTransform();

    APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PlayableCharacter[PlayerIndex], SpawnTransform);

    if (SpawnedPawn)
    {
        NewPlayer->Possess(SpawnedPawn);

        // 입력 차단 - 컨트롤러 레벨
        NewPlayer->SetIgnoreMoveInput(true);
        NewPlayer->SetIgnoreLookInput(true);

        // 캐릭터 입력 차단
        SpawnedPawn->DisableInput(nullptr);

        // 회전 및 움직임 완전 차단
        if (ANS_PlayerCharacterBase* Character = Cast<ANS_PlayerCharacterBase>(SpawnedPawn))
        {
            Character->bUseControllerRotationYaw = false;

            if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
            {
                MoveComp->DisableMovement();
                MoveComp->bOrientRotationToMovement = false;
            }
        }


        // 키 바인딩 제거 (C, F 등 작동 방지)
        if (NewPlayer->InputComponent)
        {
            NewPlayer->InputComponent->ClearActionBindings();
        }

        // 캐릭터 모델 경로 저장
        if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            FString ModelPath = GameInstance->CharacterList.IsValidIndex(PlayerIndex)
                ? GameInstance->CharacterList[PlayerIndex]
                : TEXT("");

            if (ANS_PlayerState* PlayerState = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
            {
                PlayerState->SetPlayerModelPath(ModelPath);
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s"), *SpawnedPawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed."));
    }

    // PlayerState 저장
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

    UE_LOG(LogTemp, Warning, TEXT("현재 플레이어 수: %d"), NumPlayers);

    for (APlayerState* PS : GS->PlayerArray)
    {
        if (ANS_PlayerState* MyPS = Cast<ANS_PlayerState>(PS))
        {
            UE_LOG(LogTemp, Warning, TEXT(" - %s: %s"),
                *MyPS->GetPlayerName(),
                MyPS->GetIsReady() ? TEXT("READY") : TEXT("NOT READY"));

            if (!MyPS->GetIsReady())
            {
                UE_LOG(LogTemp, Warning, TEXT("⛔ 아직 준비 안된 플레이어 존재. 종료."));
                return;
            }
        }
    }


    UE_LOG(LogTemp, Log, TEXT(" All players ready. Starting game."));
    Multicast_ShowLoadingScreen();

    FTimerHandle TravelTimer;
    GetWorld()->GetTimerManager().SetTimer(TravelTimer, this, &ANS_LobbyMode::GoToGameLevel, 10.0f, false);
}


void ANS_LobbyMode::GoToGameLevel()
{
    const FString LevelPath = TEXT("/Game/Maps/MainWorld");
    const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");
    GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
}


void ANS_LobbyMode::Multicast_ShowLoadingScreen_Implementation()
{
    if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld()) return;

    if (IsValid(GetWorld()->GetFirstPlayerController()) && GetWorld()->GetFirstPlayerController()->IsLocalController())
    {
        if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            if (GI->NS_UIManager)
            {
                GI->NS_UIManager->LoadingScreen(GetWorld()); 
            }
        }
    }
}

