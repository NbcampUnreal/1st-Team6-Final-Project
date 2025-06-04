#include "GameFlow/NS_LobbyMode.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

ANS_LobbyMode::ANS_LobbyMode()
{
    DefaultPawnClass = nullptr; // 자동 생성 방지

    static ConstructorHelpers::FClassFinder<APawn> PawnBPClass(
        TEXT("/Game/Character/WaitingRoomPawn/WaitingRoomPawn1")
    );

    if (PawnBPClass.Succeeded())
    {
        WaitingRoomPawnClass = PawnBPClass.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to load WaitingRoomPawn1 blueprint class."));
    }
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

    UE_LOG(LogTemp, Warning, TEXT("✅ Dedicated Server에서 CreateSession 호출됨"));
}

void ANS_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // PlayerIndex 부여
    int32 PlayerIndex = GetGameState<AGameState>()->PlayerArray.Num() - 1;

    // 스폰 위치 찾기
    AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
    if (!StartSpot)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for index %d"), PlayerIndex);
        return;
    }

    // Pawn 생성 + Possess
    FTransform SpawnTransform = StartSpot->GetActorTransform();
    APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform);
    if (SpawnedPawn)
    {
        NewPlayer->Possess(SpawnedPawn);
    }

    // PlayerState 인덱스 지정
    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
    {
        PS->SetPlayerIndex(PlayerIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerState cast to ANS_PlayerState failed. Class = %s"), *NewPlayer->PlayerState->GetClass()->GetName());
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
