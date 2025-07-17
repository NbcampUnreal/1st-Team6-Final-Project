#include "GameFlow/NS_LobbyMode.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/GameState.h"
#include "NS_LobbyController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

ANS_LobbyMode::ANS_LobbyMode()
{
    DefaultPawnClass = nullptr;
}

void ANS_LobbyMode::BeginPlay()
{
    Super::BeginPlay();

    // OnlineSubsystem에서 세션은 이미 생성된 상태임 (ListenServer/Host만)
    UE_LOG(LogTemp, Warning, TEXT("LobbyMode BeginPlay - ListenServer 환경"));
}

void ANS_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 1. PlayerIndex 계산 & 로그
    int32 PlayerIndex = GetGameState<AGameState>()->PlayerArray.Num() - 1;
    UE_LOG(LogTemp, Log, TEXT("LobbyMode PostLogin: PlayerIndex=%d, PawnClassesToSpawn.Num=%d"), PlayerIndex, PawnClassesToSpawn.Num());

    // 2. PawnClassesToSpawn 체크
    if (!PawnClassesToSpawn.IsValidIndex(PlayerIndex) || !PawnClassesToSpawn[PlayerIndex])
    {
        UE_LOG(LogTemp, Error, TEXT("PawnClassesToSpawn[%d]이(가) 비었거나, 배열에 없음! PawnClassesToSpawn.Num=%d"), PlayerIndex, PawnClassesToSpawn.Num());
        return;
    }

    // 3. SpawnPoint 체크
    AActor* StartSpot = FindSpawnPointByIndex(PlayerIndex);
    if (!StartSpot)
    {
        UE_LOG(LogTemp, Error, TEXT("Spawn point not found for index %d. PlayerStart 개수 부족?"), PlayerIndex);
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Found PlayerStart for index %d: %s (%s)"), PlayerIndex, *StartSpot->GetName(), *StartSpot->GetActorLocation().ToString());

    // 4. Pawn Spawn 시도
    FTransform SpawnTransform = StartSpot->GetActorTransform();
    APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClassesToSpawn[PlayerIndex], SpawnTransform);
    if (SpawnedPawn)
    {
        NewPlayer->Possess(SpawnedPawn);
        UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s from LobbyMode array (Class=%s)"), *SpawnedPawn->GetName(), *GetNameSafe(PawnClassesToSpawn[PlayerIndex]));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Pawn spawn failed. Class=%s"), *GetNameSafe(PawnClassesToSpawn[PlayerIndex]));
        return;
    }

    // 5. PlayerState 체크
    if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(NewPlayer->PlayerState))
    {
        PS->SetPlayerIndex(PlayerIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerState cast to ANS_PlayerState failed. PlayerController=%s"), *GetNameSafe(NewPlayer));
        return;
    }

    // 6. (옵션) 레디UI, 플레이어 리스트 등 갱신
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ANS_LobbyController* LC = Cast<ANS_LobbyController>(*It))
        {
            // ReadyUIInstance가 있으면 업데이트 (옵션)
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

    // (옵션) 레디UI 등 갱신
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

    // 즉시 인게임 월드로 이동 (SeamlessTravel 옵션)
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerTravel 즉시 실행: MainWorld 이동"));
        const FString LevelPath = TEXT("/Game/Maps/MainWorld");
        const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");
        GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
    });
}
