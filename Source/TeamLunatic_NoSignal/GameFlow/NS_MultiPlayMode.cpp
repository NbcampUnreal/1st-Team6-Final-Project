#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/PlayerStart.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{
    UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode Set !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!."));
}

void ANS_MultiPlayMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode BeginPlay Set !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!."));

    // 🔥 플레이어 스폰 추가
    SpawnAllPlayers();

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);

        if (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer))
        {
            if (GI->MyServerPort > 0)
            {
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

                if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
                {
                    if (PC->IsLocalController())
                    {
                        PC->SetInputMode(FInputModeGameOnly());
                        PC->bShowMouseCursor = false;
                        UE_LOG(LogTemp, Warning, TEXT("입력 모드 GameOnly로 설정 완료"));

                        if (APawn* CurrentPawn = PC->GetPawn())
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[BeginPlay] 현재 Possess 중인 Pawn: %s (%s)"), *CurrentPawn->GetName(), *CurrentPawn->GetClass()->GetName());
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[BeginPlay] 현재 Possess 중인 Pawn이 없습니다."));
                        }
                    }
                }

            }, 1.5f, false);
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

void ANS_MultiPlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
    if (!HasAuthority()) return;

    if (AController* Controller = DeadCharacter->GetController())
    {
        if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
        {
            PS->bIsAlive = false;
            UE_LOG(LogTemp, Warning, TEXT("!!! [GameMode] PlayerState->bIsAlive 를 false로 설정 완료. 클라이언트에서 OnRep 함수가 실행될 것입니다."));
        }
    }
}

void ANS_MultiPlayMode::SpawnAllPlayers()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

    int32 Index = 0;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        if (APawn* OldPawn = PC->GetPawn())
        {
            OldPawn->Destroy();
        }

        if (ANS_MainGamePlayerState* PS = Cast<ANS_MainGamePlayerState>(PC->PlayerState))
        {
            if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
            {
                const FNS_PlayerData* Data = GI->PlayerDataMap.Find(PS->GetPlayerId());
                if (!Data) continue;

                FStringAssetReference Ref(Data->CharacterModelPath);
                UClass* PawnClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *Ref.ToString()));
                if (!PawnClass)
                {
                    UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] PawnClass 로드 실패: %s"), *Ref.ToString());
                    continue;
                }

                FVector SpawnLoc = PlayerStarts.IsValidIndex(Index) ? PlayerStarts[Index]->GetActorLocation() : FVector::ZeroVector;
                FRotator SpawnRot = PlayerStarts.IsValidIndex(Index) ? PlayerStarts[Index]->GetActorRotation() : FRotator::ZeroRotator;

                APawn* NewPawn = World->SpawnActor<APawn>(PawnClass, SpawnLoc, SpawnRot);
                if (NewPawn)
                {
                    PC->Possess(NewPawn);
                    UE_LOG(LogTemp, Warning, TEXT("[SpawnAllPlayers] %s → %s 로 Possess됨"), *PC->GetName(), *NewPawn->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] Pawn 스폰 실패"));
                }
            }
        }

        Index++;
    }
}
