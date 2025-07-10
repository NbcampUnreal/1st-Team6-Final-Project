#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"
#include "NS_LobbyController.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/PlayerStart.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h"
#include "Character/NS_PlayerController.h"
#include "Engine/World.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{
}

void ANS_MultiPlayMode::BeginPlay()
{
    UsedPawnIndices.Empty();

    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        PlayerCount = 0;
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    PlayerCount++;
                }
            }
        }
        PlayerCount = FMath::Max(1, PlayerCount);
    }

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);
        // 데디/Listen 환경에서 별도 상태 동기화 필요 없음
    }
}

void ANS_MultiPlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        if (MainGamePawnClassesToSpawn.Num() == 0)
        {
            return;
        }
        TSubclassOf<APawn> ChosenPawnClass = GetUniqueRandomPawnClass();

        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        AActor* StartSpot = nullptr;
        if (PlayerStarts.Num() > 0)
        {
            const int32 RandStartSpotIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
            StartSpot = PlayerStarts[RandStartSpotIndex];
        }

        if (!StartSpot)
        {
            return;
        }

        if (ChosenPawnClass)
        {
            FActorSpawnParameters SpawnInfo;
            SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(ChosenPawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnInfo);

            if (NewPawn)
            {
                NewPlayer->Possess(NewPawn);
                int32 SelectedIndex = MainGamePawnClassesToSpawn.IndexOfByKey(ChosenPawnClass);
                if (SelectedIndex != INDEX_NONE)
                {
                    if (ANS_PlayerController* PC = Cast<ANS_PlayerController>(NewPlayer))
                    {
                        UE_LOG(LogTemp, Log, TEXT("Player %s assigned pawn index %d"), *NewPlayer->PlayerState->GetPlayerName(), SelectedIndex);
                    }
                }
                UE_LOG(LogTemp, Log, TEXT("Player %s spawned and possessed unique pawn %s (Index: %d)."),
                    *NewPlayer->PlayerState->GetPlayerName(), *NewPawn->GetName(), SelectedIndex);
            }
        }
    }
}

FVector ANS_MultiPlayMode::GetPlayerLocation_Implementation() const
{
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        if (APawn* Target = GS->TrackingTarget)
        {
            if (AController* Controller = Target->GetController())
            {
                if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
                {
                    if (PS->bIsAlive)
                    {
                        return Target->GetActorLocation();
                    }
                }
            }
        }
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    if (APawn* AlivePawn = MPS->GetPawn())
                    {
                        return AlivePawn->GetActorLocation();
                    }
                }
            }
        }
    }
    return FVector::ZeroVector;
}

void ANS_MultiPlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
    if (!HasAuthority()) return;

    if (AController* Controller = DeadCharacter->GetController())
    {
        if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
        {
            PS->bIsAlive = false;
        }
    }
}

void ANS_MultiPlayMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (!HasAuthority()) return;

    if (APlayerController* PC = Cast<APlayerController>(Exiting))
    {
        ReleasePawnIndex(PC);
    }
    CheckPlayerCountAndEndSession();
}

void ANS_MultiPlayMode::CheckPlayerCountAndEndSession()
{
    if (!HasAuthority()) return;

    int32 ConnectedPlayerCount = GetNumPlayers();

    if (ConnectedPlayerCount == 0)
    {
        // Listen 환경에서는 세션 자동 종료/정리는 필요 없으나
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 모든 플레이어 퇴장, 세션 종료 조건."));
    }
}

FVector ANS_MultiPlayMode::GetRandomPlayerLocation() const
{
    TArray<FVector> PlayerLocations;

    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
            {
                if (MPS->bIsAlive)
                {
                    if (APawn* AlivePawn = MPS->GetPawn())
                    {
                        PlayerLocations.Add(AlivePawn->GetActorLocation());
                    }
                }
            }
        }
    }

    if (PlayerLocations.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PlayerLocations.Num() - 1);
        return PlayerLocations[RandomIndex];
    }
    return FVector::ZeroVector;
}

void ANS_MultiPlayMode::OptimizedMultiplaySpawnCheck()
{
    if (GetWorld()->GetDeltaSeconds() < 0.033f)
        return;
    if (bIsGameOver)
        return;
}

TSubclassOf<APawn> ANS_MultiPlayMode::GetUniqueRandomPawnClass()
{
    if (MainGamePawnClassesToSpawn.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[GetUniqueRandomPawnClass] MainGamePawnClassesToSpawn 배열이 비어있음"));
        return nullptr;
    }
    TArray<int32> AvailableIndices;
    for (int32 i = 0; i < MainGamePawnClassesToSpawn.Num(); i++)
    {
        if (!UsedPawnIndices.Contains(i))
            AvailableIndices.Add(i);
    }
    if (AvailableIndices.Num() == 0)
    {
        int32 FallbackIndex = FMath::RandRange(0, MainGamePawnClassesToSpawn.Num() - 1);
        UsedPawnIndices.Add(FallbackIndex);
        return MainGamePawnClassesToSpawn[FallbackIndex];
    }
    int32 RandomAvailableIndex = AvailableIndices[FMath::RandRange(0, AvailableIndices.Num() - 1)];
    UsedPawnIndices.Add(RandomAvailableIndex);
    return MainGamePawnClassesToSpawn[RandomAvailableIndex];
}

void ANS_MultiPlayMode::ReleasePawnIndex(APlayerController* ExitingPlayer)
{
    if (!ExitingPlayer || !ExitingPlayer->GetPawn())
        return;

    TSubclassOf<APawn> ExitingPawnClass = ExitingPlayer->GetPawn()->GetClass();
    int32 PawnIndex = MainGamePawnClassesToSpawn.IndexOfByKey(ExitingPawnClass);

    if (PawnIndex != INDEX_NONE && UsedPawnIndices.Contains(PawnIndex))
    {
        UsedPawnIndices.Remove(PawnIndex);
        UE_LOG(LogTemp, Log, TEXT("[ReleasePawnIndex] 폰 인덱스 %d 해제. 플레이어: %s, 남은 사용 중: %d"),
            PawnIndex, *ExitingPlayer->PlayerState->GetPlayerName(), UsedPawnIndices.Num());
    }
}

void ANS_MultiPlayMode::ResetCharacterDuplicationSystem()
{
    UsedPawnIndices.Empty();
    UE_LOG(LogTemp, Log, TEXT("[ResetCharacterDuplicationSystem] 캐릭터 중복 방지 리셋. 수: %d"),
        MainGamePawnClassesToSpawn.Num());
}
