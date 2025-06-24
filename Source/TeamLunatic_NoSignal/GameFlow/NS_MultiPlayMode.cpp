#include "NS_MultiPlayMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"
#include "NS_MainGamePlayerState.h"
#include "NS_GameState.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFramework/PlayerStart.h"
#include "Zombie/NS_ZombieBase.h"

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
    
    // 랜덤한 플레이어 위치 가져오기
    FVector RandomPlayerLocation = GetRandomPlayerLocation();
    if (RandomPlayerLocation.IsZero())
    {
        // 유효한 플레이어 위치가 없으면 기본 로직 사용
        Super::CheckAndSpawnZombies();
        return;
    }
    
    // 현재 좀비 위치 확인
    TArray<AActor*> ExistingZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), ExistingZombies);
    
    // 각 스포너에 좀비가 있는지 확인하는 맵
    TMap<AANS_ZombieSpawner*, bool> SpawnerHasZombie;
    
    // 각 좀비의 가장 가까운 스포너 찾기
    for (AActor* ZombieActor : ExistingZombies)
    {
        ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(ZombieActor);
        if (!Zombie) continue;
        
        FVector ZombieLocation = Zombie->GetActorLocation();
        float ClosestDistance = MAX_FLT;
        AANS_ZombieSpawner* ClosestSpawner = nullptr;
        
        for (AANS_ZombieSpawner* Spawner : ZombieSpawnPoints)
        {
            if (!IsValid(Spawner)) continue;
            
            float Distance = FVector::Dist(ZombieLocation, Spawner->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestSpawner = Spawner;
            }
        }
        
        if (ClosestSpawner && ClosestDistance < 500.0f)
        {
            SpawnerHasZombie.Add(ClosestSpawner, true);
        }
    }
    
    // 선택된 플레이어 주변의 적합한 스포너 찾기
    TArray<AANS_ZombieSpawner*> SuitableSpawners;
    
    for (AANS_ZombieSpawner* Spawner : ZombieSpawnPoints)
    {
        if (!IsValid(Spawner) || !Spawner->bIsEnabled)
        {
            continue;
        }
        
        if (SpawnerHasZombie.Contains(Spawner) && SpawnerHasZombie[Spawner])
        {
            continue;
        }
        
        FVector SpawnerLocation = Spawner->GetActorLocation();
        float DistanceToPlayer = FVector::Dist(RandomPlayerLocation, SpawnerLocation);
        
        if (DistanceToPlayer >= MinSpawnDistance && DistanceToPlayer <= MaxSpawnDistance)
        {
            SuitableSpawners.Add(Spawner);
        }
    }
    
    // 적합한 스포너가 없으면 종료
    if (SuitableSpawners.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MultiPlayMode] 선택된 플레이어 주변에 적합한 스포너가 없습니다. 플레이어 위치: X=%.1f Y=%.1f Z=%.1f"), 
            RandomPlayerLocation.X, RandomPlayerLocation.Y, RandomPlayerLocation.Z);
        return;
    }
    
    // 스포너 목록 무작위 섞기
    for (int32 i = 0; i < SuitableSpawners.Num() - 1; ++i)
    {
        int32 SwapIndex = FMath::RandRange(i, SuitableSpawners.Num() - 1);
        if (i != SwapIndex)
        {
            SuitableSpawners.Swap(i, SwapIndex);
        }
    }
    
    // 1초당 1마리만 스폰
    SpawnZombieAtPoint(SuitableSpawners[0]);
    
    UE_LOG(LogTemp, Verbose, TEXT("[MultiPlayMode] 랜덤 플레이어 주변에 좀비 스폰 시도. 플레이어 위치: X=%.1f Y=%.1f Z=%.1f, 적합한 스포너 수: %d"), 
        RandomPlayerLocation.X, RandomPlayerLocation.Y, RandomPlayerLocation.Z, SuitableSpawners.Num());
}
