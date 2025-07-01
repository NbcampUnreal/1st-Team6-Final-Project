#include "GameFlow/NS_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h"
#include "Zombie/Zombies/NS_BasicZombie.h"
#include "Zombie/Zombies/NS_FatZombie.h"
#include "Zombie/Zombies/NS_RunnerZombie.h"
#include "Zombie/Zombies/NS_Chaser.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "GameFramework/Character.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFlow/NS_MainGamePlayerState.h"


ANS_GameModeBase::ANS_GameModeBase()
{
}

void ANS_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // 초기 액터 캐시 구축 (한 번만 실행)
    InitializeActorCaches();

    // 플레이어 수에 따라 최대 좀비 수 조정
    MaxZombieCount += PlayerCount * 15;

    // 최적화된 타이머 설정
    // 1. 필수 타이머만 안전한 주기로 활성화
    float SafeSpawnInterval = CalculateOptimizedSpawnInterval();
    GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this,
        &ANS_GameModeBase::OptimizedSpawnCheck, SafeSpawnInterval, true);

    // 2. 정리 타이머는 더 긴 주기로 (캐시 정리용)
    float SafeCleanupInterval = CalculateOptimizedCleanupInterval();
    GetWorldTimerManager().SetTimer(ZombieCleanupTimer, this,
        &ANS_GameModeBase::BatchedCleanup, SafeCleanupInterval, true);

    // 3. 캐시 정리 타이머 (무효한 참조만 제거)
    GetWorldTimerManager().SetTimer(DelayedSpawnerSearchTimer, this,
        &ANS_GameModeBase::CleanupAllCaches, 30.0f, true);


    // 제거된 좀비 카운터 초기화
    ZombiesRemoved = 0;
}

// 최적화된 스폰 주기 계산
float ANS_GameModeBase::CalculateOptimizedSpawnInterval()
{
    // 플레이어 수가 많을수록 주기를 길게, 최소 3초 보장
    float BaseInterval = ZombieSpawnInterval;
    float PlayerMultiplier = FMath::Max(1.0f, PlayerCount * 0.5f);
    return FMath::Max(BaseInterval * PlayerMultiplier, 3.0f);
}

// 최적화된 정리 주기 계산
float ANS_GameModeBase::CalculateOptimizedCleanupInterval()
{
    // 플레이어 수에 따라 10-20초 사이로 조정
    return FMath::Clamp(10.0f + (PlayerCount * 2.0f), 10.0f, 20.0f);
}

// 최적화된 스폰 체크 (기존 함수를 래핑)
void ANS_GameModeBase::OptimizedSpawnCheck()
{
    // 서버 부하 체크 - 프레임 시간이 33ms(30FPS) 이상이면 건너뜀
    if (GetWorld()->GetDeltaSeconds() < 0.033f)
    {
        return;
    }

    // 기존 스폰 로직 실행
    CheckAndSpawnZombies();
}

// 배치 처리된 좀비 정리
void ANS_GameModeBase::BatchedCleanup()
{
    // 서버 부하 체크
    if (GetWorld()->GetDeltaSeconds() < 0.033f)
    {
        return;
    }

    // 기존 정리 로직 실행
    CleanupDistantZombies();
}

// 이벤트 기반 캐싱 시스템 구현
TArray<ANS_ZombieBase*> ANS_GameModeBase::GetCachedZombies()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 정리가 필요한지 확인 (더 긴 주기)
    if (CurrentTime - LastZombieListUpdateTime > ZombieListCleanupInterval)
    {
        CleanupZombieCache();
        LastZombieListUpdateTime = CurrentTime;
    }

    // 유효한 좀비들만 반환
    TArray<ANS_ZombieBase*> ValidZombies;
    ValidZombies.Reserve(CachedZombies.Num());

    for (const TWeakObjectPtr<ANS_ZombieBase>& WeakZombie : CachedZombies)
    {
        if (WeakZombie.IsValid())
        {
            ValidZombies.Add(WeakZombie.Get());
        }
    }

    return ValidZombies;
}

TArray<ANS_PlayerCharacterBase*> ANS_GameModeBase::GetCachedPlayers()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 정리가 필요한지 확인 (더 긴 주기)
    if (CurrentTime - LastPlayerListUpdateTime > PlayerListCleanupInterval)
    {
        CleanupPlayerCache();
        LastPlayerListUpdateTime = CurrentTime;
    }

    // 유효한 플레이어들만 반환
    TArray<ANS_PlayerCharacterBase*> ValidPlayers;
    ValidPlayers.Reserve(CachedPlayers.Num());

    for (const TWeakObjectPtr<ANS_PlayerCharacterBase>& WeakPlayer : CachedPlayers)
    {
        if (WeakPlayer.IsValid())
        {
            ValidPlayers.Add(WeakPlayer.Get());
        }
    }

    return ValidPlayers;
}

TArray<AANS_ZombieSpawner*> ANS_GameModeBase::GetCachedSpawners()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 정리가 필요한지 확인 (스포너는 거의 변하지 않으므로 매우 긴 주기)
    if (CurrentTime - LastSpawnerListUpdateTime > SpawnerListCleanupInterval)
    {
        CleanupSpawnerCache();
        LastSpawnerListUpdateTime = CurrentTime;
    }

    // 유효한 스포너들만 반환
    TArray<AANS_ZombieSpawner*> ValidSpawners;
    ValidSpawners.Reserve(CachedSpawners.Num());

    for (const TWeakObjectPtr<AANS_ZombieSpawner>& WeakSpawner : CachedSpawners)
    {
        if (WeakSpawner.IsValid())
        {
            ValidSpawners.Add(WeakSpawner.Get());
        }
    }

    return ValidSpawners;
}

// 초기 캐시 구축 함수 (BeginPlay에서 한 번만 호출)
void ANS_GameModeBase::InitializeActorCaches()
{
    if (bActorCachesInitialized) return;

    // 좀비 스포너 찾기 및 캐시
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AANS_ZombieSpawner::StaticClass(), FoundSpawners);

    ZombieSpawnPoints.Empty();
    CachedSpawners.Empty(FoundSpawners.Num());

    for (AActor* Actor : FoundSpawners)
    {
        if (AANS_ZombieSpawner* Spawner = Cast<AANS_ZombieSpawner>(Actor))
        {
            ZombieSpawnPoints.Add(Spawner);
            CachedSpawners.Add(TWeakObjectPtr<AANS_ZombieSpawner>(Spawner));
        }
    }

    // 기존 좀비 찾기 및 캐시
    TArray<AActor*> ExistingZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), ExistingZombies);

    CurrentZombieCount = ExistingZombies.Num();
    CachedZombies.Empty(ExistingZombies.Num());

    for (AActor* Z : ExistingZombies)
    {
        if (IsValid(Z))
        {
            Z->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);

            if (ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(Z))
            {
                CachedZombies.Add(TWeakObjectPtr<ANS_ZombieBase>(Zombie));
            }
        }
    }

    // 플레이어 찾기 및 캐시
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_PlayerCharacterBase::StaticClass(), FoundPlayers);

    CachedPlayers.Empty(FoundPlayers.Num());
    for (AActor* Actor : FoundPlayers)
    {
        if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(Actor))
        {
            CachedPlayers.Add(TWeakObjectPtr<ANS_PlayerCharacterBase>(Player));
        }
    }

    bActorCachesInitialized = true;
}

// 이벤트 기반 캐시 관리 함수들
void ANS_GameModeBase::RegisterZombie(ANS_ZombieBase* Zombie)
{
    if (!Zombie) return;

    // 중복 등록 방지
    for (const TWeakObjectPtr<ANS_ZombieBase>& WeakZombie : CachedZombies)
    {
        if (WeakZombie.IsValid() && WeakZombie.Get() == Zombie)
        {
            return; // 이미 등록됨
        }
    }
    CachedZombies.Add(TWeakObjectPtr<ANS_ZombieBase>(Zombie));
    Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
}

void ANS_GameModeBase::UnregisterZombie(ANS_ZombieBase* Zombie)
{
    if (!Zombie) return;

    CachedZombies.RemoveAll([Zombie](const TWeakObjectPtr<ANS_ZombieBase>& WeakPtr)
    {
        return !WeakPtr.IsValid() || WeakPtr.Get() == Zombie;
    });
}

void ANS_GameModeBase::RegisterPlayer(ANS_PlayerCharacterBase* Player)
{
    if (!Player) return;

    // 중복 등록 방지
    for (const TWeakObjectPtr<ANS_PlayerCharacterBase>& WeakPlayer : CachedPlayers)
    {
        if (WeakPlayer.IsValid() && WeakPlayer.Get() == Player)
        {
            return; // 이미 등록됨
        }
    }

    CachedPlayers.Add(TWeakObjectPtr<ANS_PlayerCharacterBase>(Player));
}

void ANS_GameModeBase::UnregisterPlayer(ANS_PlayerCharacterBase* Player)
{
    if (!Player) return;

    CachedPlayers.RemoveAll([Player](const TWeakObjectPtr<ANS_PlayerCharacterBase>& WeakPtr)
    {
        return !WeakPtr.IsValid() || WeakPtr.Get() == Player;
    });
}

void ANS_GameModeBase::RegisterSpawner(AANS_ZombieSpawner* Spawner)
{
    if (!Spawner) return;

    // 중복 등록 방지
    for (const TWeakObjectPtr<AANS_ZombieSpawner>& WeakSpawner : CachedSpawners)
    {
        if (WeakSpawner.IsValid() && WeakSpawner.Get() == Spawner)
        {
            return; // 이미 등록됨
        }
    }

    CachedSpawners.Add(TWeakObjectPtr<AANS_ZombieSpawner>(Spawner));

    // ZombieSpawnPoints에도 추가 (기존 로직 유지)
    if (!ZombieSpawnPoints.Contains(Spawner))
    {
        ZombieSpawnPoints.Add(Spawner);
    }
}

void ANS_GameModeBase::UnregisterSpawner(AANS_ZombieSpawner* Spawner)
{
    if (!Spawner) return;

    CachedSpawners.RemoveAll([Spawner](const TWeakObjectPtr<AANS_ZombieSpawner>& WeakPtr)
    {
        return !WeakPtr.IsValid() || WeakPtr.Get() == Spawner;
    });

    ZombieSpawnPoints.Remove(Spawner);
}

// 캐시 정리 함수들 (무효한 참조만 제거)
void ANS_GameModeBase::CleanupZombieCache()
{
    CachedZombies.RemoveAll([](const TWeakObjectPtr<ANS_ZombieBase>& WeakPtr)
    {
        return !WeakPtr.IsValid();
    });
}

void ANS_GameModeBase::CleanupPlayerCache()
{
    CachedPlayers.RemoveAll([](const TWeakObjectPtr<ANS_PlayerCharacterBase>& WeakPtr)
    {
        return !WeakPtr.IsValid();
    });
}

void ANS_GameModeBase::CleanupSpawnerCache()
{
    CachedSpawners.RemoveAll([](const TWeakObjectPtr<AANS_ZombieSpawner>& WeakPtr)
    {
        return !WeakPtr.IsValid();
    });

    // ZombieSpawnPoints도 정리
    ZombieSpawnPoints.RemoveAll([](AANS_ZombieSpawner* Spawner)
    {
        return !IsValid(Spawner);
    });
}

// 모든 캐시 정리 함수
void ANS_GameModeBase::CleanupAllCaches()
{
    CleanupZombieCache();
    CleanupPlayerCache();
    CleanupSpawnerCache();
}

// 거리 계산 최적화 함수들
bool ANS_GameModeBase::IsWithinDistanceSquared(const FVector& Location1, const FVector& Location2, float DistanceThreshold)
{
    // 제곱근 계산을 피하기 위해 DistSquared 사용
    float DistanceThresholdSquared = DistanceThreshold * DistanceThreshold;
    return FVector::DistSquared(Location1, Location2) <= DistanceThresholdSquared;
}

TArray<ANS_ZombieBase*> ANS_GameModeBase::GetZombiesInRange(const FVector& CenterLocation, float Range)
{
    TArray<ANS_ZombieBase*> ZombiesInRange;
    TArray<ANS_ZombieBase*> AllZombies = GetCachedZombies();

    ZombiesInRange.Reserve(AllZombies.Num() / 4); // 대략적인 예상 크기

    float RangeSquared = Range * Range;
    for (ANS_ZombieBase* Zombie : AllZombies)
    {
        if (IsValid(Zombie))
        {
            if (FVector::DistSquared(CenterLocation, Zombie->GetActorLocation()) <= RangeSquared)
            {
                ZombiesInRange.Add(Zombie);
            }
        }
    }

    return ZombiesInRange;
}

// SearchForSpawnersDelayed 함수는 더 이상 필요하지 않음 (이벤트 기반 캐싱으로 대체)

// 플레이어 위치 반환
FVector ANS_GameModeBase::GetPlayerLocation_Implementation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetActorLocation();
	}
	return FVector::ZeroVector;
}

// 좀비 스폰 여부 확인 및 스폰
void ANS_GameModeBase::CheckAndSpawnZombies()
{
	// 현재 좀비 수 확인
	int32 Missing = MaxZombieCount - CurrentZombieCount;
	
	// 최대 좀비 수에 도달했으면 스폰하지 않음
	if (Missing <= 0)
	{
		return;
	}
	
	// 스포너가 없으면 종료
	if (ZombieSpawnPoints.Num() == 0)
	{
		return;
	}
	
	// 플레이어 위치 확인
	FVector PlayerLocation = GetPlayerLocation();
	
	// 적합한 스포너 찾기
	TArray<AANS_ZombieSpawner*> SuitableSpawners = FindSuitableSpawners(PlayerLocation);
	
	// 적합한 스포너가 없으면 종료
	if (SuitableSpawners.Num() <= 0)
	{
		return;
	}
	
	// 스포너 목록 무작위 섞기
	Algo::RandomShuffle(SuitableSpawners);
	
	// 한 번에 스폰할 좀비 수 계산 (최대 Missing까지)
	int32 SpawnCount = FMath::Min(ZombiesPerSpawn, Missing);
	
	// 계산된 수만큼 좀비 스폰
	for (int32 i = 0; i < SpawnCount; i++)
	{
		// 적합한 스포너가 부족하면 순환
		int32 SpawnerIndex = i % SuitableSpawners.Num();
		SpawnZombieAtPoint(SuitableSpawners[SpawnerIndex]);
	}
}

// 적합한 스포너 찾기
TArray<AANS_ZombieSpawner*> ANS_GameModeBase::FindSuitableSpawners(const FVector& PlayerLocation)
{
    TArray<AANS_ZombieSpawner*> SuitableSpawners;
    TMap<AANS_ZombieSpawner*, bool> SpawnerHasZombie;
    
    // 캐시된 좀비 위치 확인 (GetAllActorsOfClass 대신 캐시 사용)
    TArray<ANS_ZombieBase*> ExistingZombies = GetCachedZombies();

	// 월드에 좀비가 있으면
	if (ExistingZombies.Num() > 0)
	{
		// 각 좀비의 가장 가까운 스포너 찾기 (최적화된 버전)
		TArray<AANS_ZombieSpawner*> CachedSpawnerList = GetCachedSpawners();
		for (ANS_ZombieBase* Zombie : ExistingZombies)
		{
			if (!IsValid(Zombie)) continue;

			FVector ZombieLocation = Zombie->GetActorLocation();
			float ClosestDistanceSquared = MAX_FLT;
			AANS_ZombieSpawner* ClosestSpawner = nullptr;

			// 캐시된 스포너 리스트 사용 및 DistSquared로 최적화
			for (AANS_ZombieSpawner* Spawner : CachedSpawnerList)
			{
				if (!IsValid(Spawner)) continue;

				float DistanceSquared = FVector::DistSquared(ZombieLocation, Spawner->GetActorLocation());
				if (DistanceSquared < ClosestDistanceSquared)
				{
					ClosestDistanceSquared = DistanceSquared;
					ClosestSpawner = Spawner;
				}
			}

			// 좀비가 스포너 근처에 있으면 해당 스포너는 사용 중으로 표시 (500^2 = 250000)
			if (ClosestSpawner && ClosestDistanceSquared < 4000.0f)
			{
				SpawnerHasZombie.Add(ClosestSpawner, true);
			}
		}
	}
	
    // 적합한 스포너 찾기 (캐시된 리스트 사용 및 거리 계산 최적화)
    TArray<AANS_ZombieSpawner*> CachedSpawnerList = GetCachedSpawners();
    float MinSpawnDistanceSquared = MinSpawnDistance * MinSpawnDistance;
    float MaxSpawnDistanceSquared = MaxSpawnDistance * MaxSpawnDistance;

    for (AANS_ZombieSpawner* Spawner : CachedSpawnerList)
    {
        // 유효하지 않거나 비활성화된 스포너는 제외
        if (!IsValid(Spawner) || !Spawner->bIsEnabled)
        {
            continue;
        }

        // 이미 좀비가 있는 스포너는 제외
        if (SpawnerHasZombie.Contains(Spawner) && SpawnerHasZombie[Spawner])
        {
            continue;
        }

        // 플레이어와의 거리 확인 (DistSquared 사용으로 최적화)
        FVector SpawnerLocation = Spawner->GetActorLocation();
        float DistanceToPlayerSquared = FVector::DistSquared(PlayerLocation, SpawnerLocation);

        // 최소/최대 거리 범위 내에 있는 스포너만 선택
        if (DistanceToPlayerSquared >= MinSpawnDistanceSquared && DistanceToPlayerSquared <= MaxSpawnDistanceSquared)
        {
            SuitableSpawners.Add(Spawner);
        }
    }
    
    return SuitableSpawners;
}

void ANS_GameModeBase::SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnSpawner) 
{
    if (!SpawnSpawner) 
    {
        return;
    }

    // 좀비 클래스 선택
    TSubclassOf<ANS_ZombieBase> ZombieToSpawn = SelectZombieClass();
    if (!ZombieToSpawn)
    {
        return; 
    }

    // 스폰 위치 계산
    FVector SpawnLocation = GetSpawnLocationOnGround(SpawnSpawner);
    
    // 좀비 스폰
    ANS_ZombieBase* Zombie = SpawnZombieAtLocation(ZombieToSpawn, SpawnLocation);
    
    // 좀비 스폰 후처리
    if (Zombie)
    {
        RegisterSpawnedZombie(Zombie);
    }
}

// 좀비 클래스 선택 함수
TSubclassOf<ANS_ZombieBase> ANS_GameModeBase::SelectZombieClass()
{
    int32 RandNum = FMath::RandRange(0, 100);
    
    if (RandNum <= 70 && BasicZombieClass)  // 60% 기본 좀비
    {
        return BasicZombieClass;
    }
    if (RandNum <= 85 && RandNum > 70 && FatZombieClass)  // 20% 뚱 좀비
    {
        return FatZombieClass;
    }
    if (RandNum <= 100 && RandNum > 85 && RunnerZombieClass)  // 20% 러너 좀비
    {
        return RunnerZombieClass;
    }
    
    return nullptr;
}

// 지면 위 스폰 위치 계산 함수
FVector ANS_GameModeBase::GetSpawnLocationOnGround(AANS_ZombieSpawner* Spawner)
{
    // 스폰 위치 가져오기
    FVector SpawnLocation = Spawner->GetRandomSpawnLocationInBounds();

    // 지면 위에 스폰하기 위한 트레이스 수행
    FHitResult HitResult;
    FVector TraceStart = SpawnLocation + FVector(0, 0, 200);  // 더 높은 위치에서 시작
    FVector TraceEnd = SpawnLocation - FVector(0, 0, 1000);   // 더 깊이 트레이스

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(Spawner);

    // WorldStatic 채널을 사용하여 더 정확한 지면 감지
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_WorldStatic,
        QueryParams
    );

    // 지면을 찾았으면 그 위치에 스폰 (좀비 캡슐 높이 고려)
    if (bHit)
    {
        // 좀비 캡슐 컴포넌트의 절반 높이만큼 위에 스폰 (일반적으로 88-95 정도)
        return HitResult.Location + FVector(0, 0, 95);
    }
    // 지면을 찾지 못했으면 원래 스폰 위치 반환
    return SpawnLocation;
}

// 좀비 스폰 함수
ANS_ZombieBase* ANS_GameModeBase::SpawnZombieAtLocation(TSubclassOf<ANS_ZombieBase> ZombieClass, const FVector& Location)
{
    FRotator SpawnRotation = FRotator::ZeroRotator;
    FTransform SpawnTransform(SpawnRotation, Location);

    FActorSpawnParameters Params;
    // 콜리전이 있어도 강제로 스폰하되, 위치를 약간 조정
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    Params.bNoFail = true;

    ANS_ZombieBase* SpawnedZombie = GetWorld()->SpawnActor<ANS_ZombieBase>(ZombieClass, SpawnTransform, Params);

    // 스폰된 좀비가 있다면 추가 검증
    if (SpawnedZombie)
    {
    	return SpawnedZombie;
    }	
	return nullptr;   
}

// 스폰된 좀비 등록 함수
void ANS_GameModeBase::RegisterSpawnedZombie(ANS_ZombieBase* Zombie)
{
    if (!Zombie) return;

    // 이벤트 기반 캐시 등록 (OnDestroyed 바인딩 포함)
    RegisterZombie(Zombie);
    ++CurrentZombieCount;

    // 좀비 활성화 매니저에 등록
    AActor* ZombieActivationManager = UGameplayStatics::GetActorOfClass(GetWorld(), ANS_ZombieActivationManager::StaticClass());
    if (ANS_ZombieActivationManager* ZombieActivationManagerCasted = Cast<ANS_ZombieActivationManager>(ZombieActivationManager))
    {
        ZombieActivationManagerCasted->AppendSpawnZombie(Zombie);
    }
}

void ANS_GameModeBase::OnZombieDestroyed(AActor* DestroyedActor)
{
	if (IsValid(DestroyedActor))
	{
		if (ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(DestroyedActor))
		{
			--CurrentZombieCount;
			CurrentZombieCount = FMath::Max(0, CurrentZombieCount);

			// 이벤트 기반 캐시에서 제거
			UnregisterZombie(Zombie);
		}
	}
}

// 캐시된 좀비 리스트 반환 함수
TArray<ANS_ZombieBase*> ANS_GameModeBase::GetValidZombies()
{
    TArray<ANS_ZombieBase*> ValidZombies;

    // 현재 시간 확인
    float CurrentTime = GetWorld()->GetTimeSeconds();

    float ZombieListUpdateInterval = 5.0f;
    // 일정 시간마다만 캐시 정리
    if (CurrentTime - LastZombieListUpdateTime > ZombieListUpdateInterval)
    {
        // 무효한 참조 제거
        CachedZombies.RemoveAll([](const TWeakObjectPtr<ANS_ZombieBase>& WeakPtr)
        {
            return !WeakPtr.IsValid();
        });

        LastZombieListUpdateTime = CurrentTime;
    }

    // 유효한 좀비들만 반환
    for (const TWeakObjectPtr<ANS_ZombieBase>& WeakZombie : CachedZombies)
    {
        if (WeakZombie.IsValid())
        {
            ValidZombies.Add(WeakZombie.Get());
        }
    }

    return ValidZombies;
}

// 플레이어로부터 너무 멀리 있는 좀비 제거 함수
void ANS_GameModeBase::CleanupDistantZombies()
{
    // 플레이어 위치 가져오기
    FVector PlayerLocation = GetPlayerLocation();
    if (PlayerLocation.IsZero())
    {
        return; // 유효한 플레이어 위치가 없으면 종료
    }

    // 캐시된 좀비 리스트 사용 (GetAllActorsOfClass 대신!)
    TArray<ANS_ZombieBase*> AllZombies = GetCachedZombies();

    int32 DestroyedCount = 0;
    float ZombieDestroyDistanceSquared = ZombieDestroyDistance * ZombieDestroyDistance;

    // 각 좀비의 거리 확인 및 제거 (최적화된 버전)
    for (ANS_ZombieBase* Zombie : AllZombies)
    {
        if (!IsValid(Zombie))
        {
            continue;
        }

        // 체이서 좀비는 제거하지 않음
        if (Cast<ANS_Chaser>(Zombie))
        {
            continue;
        }

        // 플레이어와의 거리 계산 (DistSquared 사용으로 최적화)
        float DistanceSquared = FVector::DistSquared(PlayerLocation, Zombie->GetActorLocation());

        // 설정된 거리보다 멀리 있으면 제거 (DistSquared 비교로 최적화)
        if (DistanceSquared > ZombieDestroyDistanceSquared)
        {
            // 좀비 제거 (OnZombieDestroyed 이벤트가 자동으로 호출됨)
            Zombie->Destroy();
            DestroyedCount++;
        }
    }

    if (DestroyedCount > 0)
    {
        
    }
}



void ANS_GameModeBase::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
    // 기본 구현: 플레이어가 죽었을 때의 기본 처리
    if (!DeadCharacter)
    {
        return;
    }
    // 플레이어 상태 업데이트
    if (AController* Controller = DeadCharacter->GetController())
    {
        if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
        {
            PS->bIsAlive = false;
        }
    }
}
