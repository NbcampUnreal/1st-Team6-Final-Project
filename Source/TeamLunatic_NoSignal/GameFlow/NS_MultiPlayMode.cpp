#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "NS_GameInstance.h"
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

    if (ANS_GameState* GameState = Cast<ANS_GameState>(GetWorld()->GetGameState()))
    {
        for (APlayerState* PS : GameState->PlayerArray)
        {
            if (ANS_MainGamePlayerState* PlayerState = Cast<ANS_MainGamePlayerState>(PS))
            {
                // PlayerStart 위치
                if (!StartPoints.IsValidIndex(PlayerIndex))
                {
                    UE_LOG(LogTemp, Warning, TEXT("PlayerStart 인덱스 초과, 마지막 지점 재사용"));
                    PlayerIndex = StartPoints.Num() - 1;
                }

				// 플레이어 상태에서 데이터 로드
                APlayerController* PlayerController = PlayerState->GetPlayerController();
                PlayerState->LoadPlayerData(); // 플레이어 상태에서 데이터 로드

                if (PlayerController && StartPoints.IsValidIndex(PlayerIndex))
                {
                    FVector SpawnLocation = StartPoints[PlayerIndex]->GetActorLocation();
                    FRotator SpawnRotation = StartPoints[PlayerIndex]->GetActorRotation();

					TSubclassOf<APawn> PlayerPawnClass = nullptr;

                    //경로로 부터 BP 가져오기
                    ConstructorHelpers::FClassFinder<APawn> PawnBPClass(*PlayerState->GetPlayerModelPath());
                    if (PawnBPClass.Succeeded())
                    {
                        PlayerPawnClass = PawnBPClass.Class;

                        // Pawn 생성 및 Possess
                        FActorSpawnParameters Params;
                        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                        APawn* NewPawn = World->SpawnActor<APawn>(PlayerPawnClass, SpawnLocation, SpawnRotation, Params);
                        if (NewPawn)
                        {
                            PlayerController->Possess(NewPawn);
                            UE_LOG(LogTemp, Log, TEXT("플레이어 %d 스폰 완료: %s"), PlayerIndex, *NewPawn->GetName());
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to load player pawn class: %s"), *PlayerState->GetPlayerModelPath());
                        continue; // 다음 플레이어로 넘어감
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerState to ANS_PlayerState!"));
                }
            }

            PlayerIndex++;
        }
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
