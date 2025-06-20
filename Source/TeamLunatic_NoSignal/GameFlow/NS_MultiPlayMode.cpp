#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "NS_GameInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/NS_PlayerController.h" 
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
        GI->SetGameModeType(EGameModeType::MultiPlayMode);
        if (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer))
        {
            if (GI->MyServerPort > 0) // 유효한 서버 포트가 있는 경우에만 요청
            {
                // 세션의 상태를 "in_game"으로 변경
                GI->RequestUpdateSessionStatus(GI->MyServerPort, TEXT("in_game"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] MyServerPort가 유효하지 않아 매치메이킹 서버에 세션 상태 업데이트 요청을 보낼 수 없습니다."));
            }
        }

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


void ANS_MultiPlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) \
{
    // 이 함수는 서버에서만 호출되어야 함 (GameMode는 서버에서만 존재)
    if (!GetWorld()->IsNetMode(NM_DedicatedServer) && !GetWorld()->IsNetMode(NM_ListenServer))
    {
        return;
    }

    // bIsGameOver는 전체 게임 종료를 의미할 때만 사용.
    // 여기서는 죽은 플레이어에게만 UI를 띄우므로, 전체 게임 오버 플래그는 필요에 따라 조절.
    // if (bIsGameOver) return; // 만약 모든 플레이어가 죽어야 전체 게임 오버라면 이 로직은 나중에 활성화

    if (DeadCharacter && DeadCharacter->IsPlayerControlled())
    {
        ANS_PlayerController* DeadPlayerController = Cast<ANS_PlayerController>(DeadCharacter->GetController());
        if (DeadPlayerController)
        {
            // 사망한 플레이어 컨트롤러에게 게임 오버 UI를 띄우도록 지시
            HandleGameOverMultiplayer(DeadPlayerController);

            // 여기에 멀티플레이어 게임의 '게임 오버' 조건 검사
            bool bAllPlayersDead = true;
            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                APlayerController* PC = It->Get();
                if (PC)
                {
                    // PlayerController가 유효하고, 해당 PlayerController에 유효한 PlayerState가 있는지 확인
                    if (ANS_MainGamePlayerState* PS = Cast<ANS_MainGamePlayerState>(PC->PlayerState))
                    {
                        // PlayerState의 bIsAlive 값으로 플레이어의 생존 여부 확인
                        if (PS->bIsAlive) // 만약 이 플레이어의 PlayerState가 아직 살아있다고 보고하면
                        {
                            bAllPlayersDead = false; // 모든 플레이어가 죽은 것은 아님
                            break; // 더 이상 확인할 필요 없이 루프 종료
                        }
                    }
                    else
                    {
                        // PlayerController는 있으나 PlayerState가 유효하지 않거나 ANS_MainGamePlayerState가 아님
                        UE_LOG(LogTemp, Warning, TEXT("Multiplayer: PlayerController %s has invalid or non-ANS_MainGamePlayerState. Cannot determine alive status."), *PC->GetName());
                        // 이 경우, 해당 플레이어를 '살아있는' 것으로 간주하여 bAllPlayersDead를 false로 설정하거나,
                        // 아니면 유효하지 않은 PlayerState를 가진 플레이어는 무시하도록 로직을 정교화할수있고
                        // 여기서는 일단 '살아있는 것으로 간주'하는 보수적인 접근을 위해 아래 줄을 추가할 수 있
                        // bAllPlayersDead = false;
                        // break;
                    }
                }
            }

            if (bAllPlayersDead)
            {
                bIsGameOver = true; // 모든 플레이어가 죽었으니 전체 게임 오버 플래그 설정
                // 모든 클라이언트에게 게임 종료 (메인 메뉴 이동 등) 지시 가능
                UE_LOG(LogTemp, Warning, TEXT("Multiplayer: All players are dead. Full game over."));
                // 모든 클라이언트에 게임 종료 (예: MainTitle 맵으로 이동) RPC 호출 로직 추가
                // 예: GetWorld()->ServerTravel("/Game/Maps/MainTitle", false); // 서버에서 직접 맵 이동
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("OnPlayerCharacterDied_Implementation: Failed to cast DeadCharacter's Controller to ANS_PlayerController.")); // 로그 메시지 수정
            }
        }
    }

}

// 사망한 특정 플레이어 컨트롤러에게만 게임 오버 UI를 띄우도록 RPC 호출
void ANS_MultiPlayMode::HandleGameOverMultiplayer(APlayerController* DeadPlayerController)
{
    if (!DeadPlayerController) return;

    ANS_PlayerController* TargetPlayerController = Cast<ANS_PlayerController>(DeadPlayerController);
    if (TargetPlayerController)
    {
        // 클라이언트 RPC를 통해 해당 플레이어에게 게임 오버 UI 표시 및 입력 모드 변경 지시
        TargetPlayerController->ClientSetGameOverInputMode(); // 입력 차단 및 마우스 활성화
        TargetPlayerController->ClientShowGameOverUI();       // 게임 오버 UI 표시
        UE_LOG(LogTemp, Warning, TEXT("Multiplayer: Sent Game Over UI to player: %s"), *TargetPlayerController->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Multiplayer: Failed to cast DeadPlayerController to ANS_PlayerController. Make sure DeadPlayerController is of type ANS_PlayerController."));
    }
}
