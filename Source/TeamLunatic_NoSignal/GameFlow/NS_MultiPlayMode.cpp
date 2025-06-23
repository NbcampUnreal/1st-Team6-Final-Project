#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
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

    SpawnAllPlayers();

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);

        if (IsRunningDedicatedServer())
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
            UE_LOG(LogTemp, Warning, TEXT("!!! [GameMode] PlayerState->bIsAlive 를 false로 설정 완료. 클라이언트에서 OnRep 함수가 실행될 것입니다."));
        }
    }
}

void ANS_MultiPlayMode::SpawnAllPlayers()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance());
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] UNS_GameInstance를 찾을 수 없습니다!"));
        return;
    }

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

    int32 SpawnPointIndex = 0; 
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
            int32 PlayerCharacterIndex = PS->PlayerIndex;
            TSubclassOf<APawn> PawnClass = nullptr;

            if (GI->AvailableCharacterClasses.IsValidIndex(PlayerCharacterIndex))
            {
                PawnClass = GI->AvailableCharacterClasses[PlayerCharacterIndex];
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] GameInstance의 AvailableCharacterClasses 배열에 유효하지 않은 인덱스(%d)입니다. 배열 크기: %d"), PlayerCharacterIndex, GI->AvailableCharacterClasses.Num());
                continue;
            }

            if (!PawnClass)
            {
                UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] 인덱스 %d에 해당하는 PawnClass가 Null입니다."), PlayerCharacterIndex);
                continue;
            }

            FVector SpawnLoc = PlayerStarts.IsValidIndex(SpawnPointIndex) ? PlayerStarts[SpawnPointIndex]->GetActorLocation() : FVector::ZeroVector;
            FRotator SpawnRot = PlayerStarts.IsValidIndex(SpawnPointIndex) ? PlayerStarts[SpawnPointIndex]->GetActorRotation() : FRotator::ZeroRotator;

            APawn* NewPawn = World->SpawnActor<APawn>(PawnClass, SpawnLoc, SpawnRot);
            if (NewPawn)
            {
                PC->Possess(NewPawn);
                UE_LOG(LogTemp, Warning, TEXT("[SpawnAllPlayers] %s → %s 로 Possess됨 (인덱스 기반 스폰)"), *PC->GetName(), *NewPawn->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[SpawnAllPlayers] Pawn 스폰 실패"));
            }
        }

        SpawnPointIndex++;
    }
}
