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


ANS_GameModeBase::ANS_GameModeBase()
{
    // 기본 설정
    PlayerCount = 1;
    ZombiesPerSpawn = 2;  // 한 번에 2마리씩 스폰
    ZombieSpawnInterval = 3.0f;  // 3초마다 스폰
    MinSpawnDistance = 4000.0f;
    MaxSpawnDistance = 8000.0f;
    ZombieDestroyDistance = 8001.0f;
}

void ANS_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
    // 좀비 스포너 찾기
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AANS_ZombieSpawner::StaticClass(), FoundActors); 
    
    ZombieSpawnPoints.Empty(); 
    
    // 모든 스포너 추가
    for (AActor* Actor : FoundActors)
    {
        if (AANS_ZombieSpawner* Spawner = Cast<AANS_ZombieSpawner>(Actor))
        {
            ZombieSpawnPoints.Add(Spawner);
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 스포너 수: %d"), ZombieSpawnPoints.Num());
    
    // 현재 레벨의 좀비 액터 찾기
    TArray<AActor*> ExistingZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), ExistingZombies);

    CurrentZombieCount = ExistingZombies.Num();
    
    // 기존 좀비 파괴 이벤트 바인딩
    for (AActor* Z : ExistingZombies)
    {
        if (IsValid(Z))
        {
            Z->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
        }
    }

    // 플레이어 수에 따라 최대 좀비 수 조정
    MaxZombieCount = 40 * PlayerCount;
    UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 플레이어 수(%d)에 따라 최대 좀비 수를 %d로 설정"), 
        PlayerCount, MaxZombieCount);

    // 좀비 스폰 타이머 설정 - 3초마다 스폰
    GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this, &ANS_GameModeBase::CheckAndSpawnZombies, ZombieSpawnInterval, true);
    UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 스폰 타이머 설정 완료 (%.1f초 간격, 한 번에 %d마리)"), 
        ZombieSpawnInterval, ZombiesPerSpawn);
    
    // 지연된 스포너 검색 타이머 설정 (3초 후 실행)
    GetWorldTimerManager().SetTimer(DelayedSpawnerSearchTimer, this, &ANS_GameModeBase::SearchForSpawnersDelayed, 3.0f, false);

    // 좀비 정리 타이머 설정 (5초마다 실행)
    GetWorldTimerManager().SetTimer(ZombieCleanupTimer, this, &ANS_GameModeBase::CleanupDistantZombies, 5.0f, true);
    UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 정리 타이머 설정 완료 (5초마다 실행)"));
    
    // 좀비 디버그 타이머 설정 (1초마다 실행)
    GetWorldTimerManager().SetTimer(ZombieDebugTimerHandle, this, &ANS_GameModeBase::DebugZombieDistances, 1.0f, true);
    UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 디버그 타이머 설정 완료 (1초마다 실행)"));
    
    // 제거된 좀비 카운터 초기화
    ZombiesRemoved = 0;
}

void ANS_GameModeBase::SearchForSpawnersDelayed()
{
	// 좀비 스포너 다시 찾기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AANS_ZombieSpawner::StaticClass(), FoundActors); 
	
	int32 NewSpawnersAdded = 0;
	
	// 새로운 스포너 추가
	for (AActor* Actor : FoundActors)
	{
		if (AANS_ZombieSpawner* Spawner = Cast<AANS_ZombieSpawner>(Actor))
		{
			// 이미 추가된 스포너인지 확인
			bool bAlreadyAdded = false;
			for (AANS_ZombieSpawner* ExistingSpawner : ZombieSpawnPoints)
			{
				if (ExistingSpawner == Spawner)
				{
					bAlreadyAdded = true;
					break;
				}
			}
			
			// 새로운 스포너만 추가
			if (!bAlreadyAdded)
			{
				ZombieSpawnPoints.Add(Spawner);
				NewSpawnersAdded++;
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 지연 검색으로 %d개의 새 스포너 추가됨, 총 스포너 수: %d"), 
		NewSpawnersAdded, ZombieSpawnPoints.Num());
}

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
		static int32 LogCounter = 0;
		if (LogCounter++ % 30 == 0)  // 30번에 한 번만 로그 출력
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameMode] 등록된 스포너가 없습니다. 스폰 불가능"));
			SearchForSpawnersDelayed();
		}
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
	
	UE_LOG(LogTemp, Verbose, TEXT("[GameMode] %d마리 좀비 스폰 완료, 현재 좀비 %d/%d"), 
		SpawnCount, CurrentZombieCount, MaxZombieCount);
}

// 적합한 스포너 찾기
TArray<AANS_ZombieSpawner*> ANS_GameModeBase::FindSuitableSpawners(const FVector& PlayerLocation)
{
    TArray<AANS_ZombieSpawner*> SuitableSpawners;
    TMap<AANS_ZombieSpawner*, bool> SpawnerHasZombie;
    
    // 현재 좀비 위치 확인
    TArray<AActor*> ExistingZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), ExistingZombies);

	// 월드에 좀비가 있으면
	if (ExistingZombies.Num() > 0)
	{
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
        
			// 좀비가 스포너 근처에 있으면 해당 스포너는 사용 중으로 표시
			if (ClosestSpawner && ClosestDistance < 500.0f)
			{
				SpawnerHasZombie.Add(ClosestSpawner, true);
			}
		}
	}
	
    // 적합한 스포너 찾기
    for (AANS_ZombieSpawner* Spawner : ZombieSpawnPoints) 
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
        
        // 플레이어와의 거리 확인
        FVector SpawnerLocation = Spawner->GetActorLocation();
        float DistanceToPlayer = FVector::Dist(PlayerLocation, SpawnerLocation);

        // 최소/최대 거리 범위 내에 있는 스포너만 선택
        if (DistanceToPlayer >= MinSpawnDistance && DistanceToPlayer <= MaxSpawnDistance)
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
    int32 RandNum = FMath::RandRange(1, 100);
    
    if (RandNum <= 60 && BasicZombieClass)  // 60% 기본 좀비
    {
        return BasicZombieClass;
    }
    else if (RandNum <= 95 && FatZombieClass)  // 35% 뚱 좀비
    {
        return FatZombieClass;
    }
    else if (RandNum <= 100 && RunnerZombieClass)  // 5% 러너 좀비
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
    FVector TraceStart = SpawnLocation + FVector(0, 0, 100);  // 약간 위에서 시작
    FVector TraceEnd = SpawnLocation - FVector(0, 0, 500);    // 충분히 아래까지 트레이스
    
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(Spawner);
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );
    
    // 지면을 찾았으면 그 위치에 스폰
    if (bHit)
    {
        return HitResult.Location + FVector(0, 0, 10);  // 지면 위 10 유닛에 위치
    }
    
    return SpawnLocation;
}

// 좀비 스폰 함수
ANS_ZombieBase* ANS_GameModeBase::SpawnZombieAtLocation(TSubclassOf<ANS_ZombieBase> ZombieClass, const FVector& Location)
{
    FRotator SpawnRotation = FRotator::ZeroRotator;
    FTransform SpawnTransform(SpawnRotation, Location);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    Params.bNoFail = true;
    
    return GetWorld()->SpawnActor<ANS_ZombieBase>(ZombieClass, SpawnTransform, Params);
}

// 스폰된 좀비 등록 함수
void ANS_GameModeBase::RegisterSpawnedZombie(ANS_ZombieBase* Zombie)
{
    if (!Zombie) return;
    
    // 좀비 파괴 이벤트 바인딩
    Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
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
		if (DestroyedActor->IsA(ANS_ZombieBase::StaticClass()))
		{
			--CurrentZombieCount; 
			CurrentZombieCount = FMath::Max(0, CurrentZombieCount); 
		}
	}
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
    
    // 모든 좀비 찾기
    TArray<AActor*> AllZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), AllZombies);
    
    int32 DestroyedCount = 0;
    
    // 각 좀비의 거리 확인 및 제거
    for (AActor* ZombieActor : AllZombies)
    {
        if (!IsValid(ZombieActor))
        {
            continue;
        }
        
        // 체이서 좀비는 제거하지 않음
        if (Cast<ANS_Chaser>(ZombieActor))
        {
            continue;
        }
        
        // 플레이어와의 거리 계산
        float Distance = FVector::Dist(PlayerLocation, ZombieActor->GetActorLocation());
        
        // 거리 디버깅
        if (Distance > ZombieDestroyDistance * 0.9f)  // 90% 이상 거리에 있는 좀비 로그
        {
            UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 거리: %.2f (제거 거리: %.2f)"), Distance, ZombieDestroyDistance);
        }
        
        // 설정된 거리보다 멀리 있으면 제거
        if (Distance > ZombieDestroyDistance)
        {
            // 좀비 제거 (OnZombieDestroyed 이벤트가 자동으로 호출됨)
            UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 제거: 거리 %.2f"), Distance);
            ZombieActor->Destroy();
            DestroyedCount++;
        }
    }
    
    if (DestroyedCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 거리가 먼 좀비 %d마리 제거됨"), DestroyedCount);
    }
}

// 좀비 거리 디버그 함수
void ANS_GameModeBase::DebugZombieDistances()
{
    // 플레이어 위치 가져오기
    FVector PlayerLocation = GetPlayerLocation();
    
    // 플레이어 위치가 유효하지 않으면 종료
    if (PlayerLocation.IsZero())
    {
        return;
    }
    
    // 현재 레벨의 모든 좀비 찾기
    TArray<AActor*> AllZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), AllZombies);
    
    // 거리별 좀비 수 초기화
    ZombiesInCloseRange = 0;
    ZombiesInMidRange = 0;
    
    // 각 좀비의 거리 확인
    for (const AActor* ZombieActor : AllZombies)
    {
        if (!IsValid(ZombieActor))
        {
            continue;
        }
        
        // 플레이어와의 거리 계산 및 거리에 따라 카운트
        if (const float Distance = FVector::Dist(PlayerLocation, ZombieActor->GetActorLocation()); Distance <= 4000.0f)
        {
            ZombiesInCloseRange++;
        }
        else if (Distance <= 8000.0f)
        {
            ZombiesInMidRange++;
        }
    }
    
    // 디버그 로그 출력
   /* UE_LOG(LogTemp, Warning, TEXT("[좀비 디버그] 총 좀비 수: %d"), AllZombies.Num());
    UE_LOG(LogTemp, Warning, TEXT("[좀비 디버그] 4000 이내 좀비: %d"), ZombiesInCloseRange);
    UE_LOG(LogTemp, Warning, TEXT("[좀비 디버그] 4000-8000 사이 좀비: %d"), ZombiesInMidRange);
    UE_LOG(LogTemp, Warning, TEXT("[좀비 디버그] 8000 초과 좀비: %d"), AllZombies.Num() - ZombiesInCloseRange - ZombiesInMidRange);
    UE_LOG(LogTemp, Warning, TEXT("[좀비 디버그] 누적 제거된 좀비: %d"), ZombiesRemoved);*/
}
