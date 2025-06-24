#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/PlayerStart.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h" // RandomShuffle 함수를 위한 헤더 추가

ANS_MultiPlayMode::ANS_MultiPlayMode()
{
    UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode Set !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!."));
}

void ANS_MultiPlayMode::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출 (좀비 스포너 초기화 등)
    ANS_GameModeBase::BeginPlay();
    
    // 기존 MultiPlayMode의 BeginPlay 로직
    UE_LOG(LogTemp, Warning, TEXT("MultiPlayMode BeginPlay 실행"));
    
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
    
    // 좀비 스폰 타이머 설정 (기존 타이머 제거 후 새로 설정)
    GetWorldTimerManager().ClearTimer(ZombieSpawnTimer);
    GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this, &ANS_MultiPlayMode::CheckAndSpawnZombies, 1.0f, true);
    
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 스폰 타이머 설정 완료"));
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

// 랜덤한 살아있는 플레이어의 위치를 반환하는 함수
FVector ANS_MultiPlayMode::GetRandomPlayerLocation() const
{
    TArray<FVector> PlayerLocations;
    
    if (const ANS_GameState* GS = GetGameState<ANS_GameState>())
    {
        // 게임 스테이트의 모든 플레이어 상태를 순회하여 살아있는 플레이어 위치 수집
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
    
    // 살아있는 플레이어가 있으면 랜덤하게 한 명 선택
    if (PlayerLocations.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PlayerLocations.Num() - 1);
        return PlayerLocations[RandomIndex];
    }
    
    // 살아있는 플레이어가 없으면 기본 위치 반환
    return FVector::ZeroVector;
}

// 좀비 스폰 체크 함수 오버라이드
void ANS_MultiPlayMode::CheckAndSpawnZombies()
{
    // 게임 오버 상태면 좀비 스폰 중단
    if (bIsGameOver)
    {
        return;
    }
    
    // 현재 좀비 수가 최대치에 도달했는지 확인
    int32 Missing = MaxZombieCount - CurrentZombieCount;
    if (Missing <= 0)
    {
        return;
    }
    
    // 살아있는 플레이어 위치 수집
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
    
    // 살아있는 플레이어가 없으면 기본 로직 사용
    if (PlayerLocations.Num() <= 0)
    {
        Super::CheckAndSpawnZombies();
        return;
    }
    
    // 각 플레이어 주변에 좀비 스폰 (최대 플레이어 수만큼)
    int32 SpawnedCount = 0;
    for (const FVector& PlayerLocation : PlayerLocations)
    {
        // 최대 스폰 수에 도달했는지 확인
        if (SpawnedCount >= Missing)
        {
            break;
        }
        
        // 해당 플레이어 주변의 적합한 스포너 찾기
        TArray<AANS_ZombieSpawner*> SuitableSpawners = FindSuitableSpawners(PlayerLocation);
        
        // 적합한 스포너가 없으면 다음 플레이어로
        if (SuitableSpawners.Num() <= 0)
        {
            continue;
        }
        
        // 스포너 목록 무작위 섞기
        Algo::RandomShuffle(SuitableSpawners);
        
        // 해당 플레이어 주변에 1마리 스폰
        SpawnZombieAtPoint(SuitableSpawners[0]);
        SpawnedCount++;
    }
}

// 플레이어로부터 너무 멀리 있는 좀비 제거 함수 (멀티플레이 버전)
void ANS_MultiPlayMode::CleanupDistantZombies()
{
    // 함수 호출 확인 로그
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] CleanupDistantZombies 함수 호출됨"));
    
    // 살아있는 플레이어 위치 수집
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
    
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 살아있는 플레이어 수: %d"), PlayerLocations.Num());
    
    // 살아있는 플레이어가 없으면 기본 로직 사용
    if (PlayerLocations.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 살아있는 플레이어가 없어 기본 로직 사용"));
        Super::CleanupDistantZombies();
        return;
    }
    
    // 현재 레벨의 모든 좀비 찾기
    TArray<AActor*> AllZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), AllZombies);
    
    UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 현재 좀비 수: %d"), AllZombies.Num());
    
    int32 DestroyedCount = 0;
    
    // 각 좀비의 거리 확인 및 제거
    for (AActor* ZombieActor : AllZombies)
    {
        if (!IsValid(ZombieActor))
        {
            continue;
        }
        
        // 모든 플레이어와의 최소 거리 계산
        float MinDistance = MAX_FLT;
        for (const FVector& PlayerLoc : PlayerLocations)
        {
            float Distance = FVector::Dist(PlayerLoc, ZombieActor->GetActorLocation());
            MinDistance = FMath::Min(MinDistance, Distance);
        }
        
        // 거리 디버깅
        if (MinDistance > ZombieDestroyDistance * 0.9f)  // 90% 이상 거리에 있는 좀비 로그
        {
            UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 거리: %.2f (제거 거리: %.2f)"), MinDistance, ZombieDestroyDistance);
        }
        
        // 모든 플레이어로부터 설정된 거리보다 멀리 있으면 제거
        if (MinDistance > ZombieDestroyDistance)
        {
            // 좀비 제거 (OnZombieDestroyed 이벤트가 자동으로 호출됨)
            UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 좀비 제거: 거리 %.2f"), MinDistance);
            ZombieActor->Destroy();
            DestroyedCount++;
        }
    }
    
    // 제거된 좀비가 있으면 로그 출력
    if (DestroyedCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] %d개의 멀리 있는 좀비 제거됨"), DestroyedCount);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("[MultiPlayMode] 제거할 좀비가 없음"));
    }
}
