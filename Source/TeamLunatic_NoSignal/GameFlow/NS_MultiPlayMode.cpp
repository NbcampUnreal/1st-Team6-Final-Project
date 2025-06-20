#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "NS_GameInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{

}

void ANS_MultiPlayMode::BeginPlay()
{
    Super::BeginPlay();

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = GI->GetUIManager())
        {
            UIManager->LoadingScreen(GetWorld());
            FTimerHandle Timer;
            GetWorld()->GetTimerManager().SetTimer(Timer, [UIManager, this]()
            {
                UIManager->CloseLoadingUI();
                UIManager->ShowPlayerHUDWidget(GetWorld());

            }, 1.5f, false);
        }
    }

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

    if (ANS_GameState* NSGameState = Cast<ANS_GameState>(GetWorld()->GetGameState()))
    {
        for (APlayerState* PS : NSGameState->PlayerArray)
        {
            if (ANS_MainGamePlayerState* PlayerState = Cast<ANS_MainGamePlayerState>(PS))
            {
                APlayerController* PlayerController = PlayerState->GetPlayerController();
                PlayerState->LoadPlayerData();

                if (PlayerController && StartPoints.IsValidIndex(PlayerIndex))
                {
                    FVector SpawnLocation = StartPoints[PlayerIndex]->GetActorLocation();
                    FRotator SpawnRotation = StartPoints[PlayerIndex]->GetActorRotation();

                    TSubclassOf<APawn> PlayerPawnClass = nullptr;

                    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
                    {
                        int32 PlayerId = PlayerState->GetPlayerId();
                        if (FNS_PlayerData* FoundData = GI->PlayerDataMap.Find(PlayerId))
                        {
                            FStringAssetReference AssetRef(FoundData->CharacterModelPath);
                            PlayerPawnClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *AssetRef.ToString()));
                        }
                    }

                    if (PlayerPawnClass)
                    {
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
                        UE_LOG(LogTemp, Warning, TEXT("Pawn 클래스를 찾을 수 없습니다 (PlayerId: %d)"), PlayerState->GetPlayerId());
                    }
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


