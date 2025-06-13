#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{

}

void ANS_MultiPlayMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnAllPlayers();
}

void ANS_MultiPlayMode::SpawnAllPlayers()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // PlayerStart 수집
    TArray<AActor*> StartPoints;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), StartPoints);

    if (StartPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerStart가 존재하지 않습니다."));
        return;
    }

    int32 PlayerIndex = 0;

    // PlayerController 순회
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        // PlayerStart 위치
        if (!StartPoints.IsValidIndex(PlayerIndex))
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerStart 인덱스 초과, 마지막 지점 재사용"));
            PlayerIndex = StartPoints.Num() - 1;
        }

        FVector SpawnLocation = StartPoints[PlayerIndex]->GetActorLocation();
        FRotator SpawnRotation = StartPoints[PlayerIndex]->GetActorRotation();

        // PlayerState → 선택 캐릭터 확인
        TSubclassOf<APawn> PawnClassToSpawn = nullptr;
        if (ANS_MainGamePlayerState* PS = Cast<ANS_MainGamePlayerState>(PC->PlayerState))
        {
            PawnClassToSpawn = PS->SelectedPawnClass;
        }

        if (!PawnClassToSpawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("선택된 PawnClass가 없습니다. 스폰 생략"));
            continue;
        }

        // Pawn 생성 및 Possess
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APawn* NewPawn = World->SpawnActor<APawn>(PawnClassToSpawn, SpawnLocation, SpawnRotation, Params);
        if (NewPawn)
        {
            PC->Possess(NewPawn);
            UE_LOG(LogTemp, Log, TEXT("플레이어 %d 스폰 완료: %s"), PlayerIndex, *NewPawn->GetName());
        }

        PlayerIndex++;
    }
}


FVector ANS_MultiPlayMode::GetPlayerLocation_Implementation() const
{
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        if (APawn* Target = GS->TrackingTarget)
        {
            return Target->GetActorLocation();
        }
    }

    return FVector::ZeroVector;
}
