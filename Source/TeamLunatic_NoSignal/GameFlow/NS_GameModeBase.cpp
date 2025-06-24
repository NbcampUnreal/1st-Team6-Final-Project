#include "GameFlow/NS_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h"
#include "Zombie/Zombies/NS_BasicZombie.h"
#include "Zombie/Zombies/NS_FatZombie.h"
#include "Zombie/Zombies/NS_RunnerZombie.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "GameFramework/Character.h"


void ANS_GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 게임모드 초기화 시작 - %s"), *GetName());
	
	// 좀비 클래스 확인
	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 좀비 클래스 확인: Basic=%s, Fat=%s, Runner=%s"),
		BasicZombieClass ? TEXT("설정됨") : TEXT("NULL"),
		FatZombieClass ? TEXT("설정됨") : TEXT("NULL"),
		RunnerZombieClass ? TEXT("설정됨") : TEXT("NULL"));
	
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

	// 좀비 스폰 타이머 설정
	GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this, &ANS_GameModeBase::CheckAndSpawnZombies, 1.0f, true);
	
	// 지연된 스포너 검색 타이머 설정 (3초 후 실행)
	GetWorldTimerManager().SetTimer(DelayedSpawnerSearchTimer, this, &ANS_GameModeBase::SearchForSpawnersDelayed, 3.0f, false);
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
	static int32 LogCounter = 0;
	bool bDetailedLog = (LogCounter++ % 30 == 0);  // 30번에 한 번만 로그 출력
    
    // 현재 좀비 수 확인
    int32 Missing = MaxZombieCount - CurrentZombieCount;
    
    if (bDetailedLog)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] 현재 좀비 수: %d, 최대: %d, 부족: %d, 스포너 수: %d"), 
            CurrentZombieCount, MaxZombieCount, Missing, ZombieSpawnPoints.Num());
    }
    
    if (Missing <= 0)
    {
        return;
    }
    
    // 스포너가 없으면 종료
    if (ZombieSpawnPoints.Num() == 0)
    {
        if (bDetailedLog)
        {
            UE_LOG(LogTemp, Error, TEXT("[GameModeBase] 등록된 스포너가 없습니다! 스폰 불가능"));
            SearchForSpawnersDelayed();
        }
        return;
    }
    
    // 플레이어 위치 확인
    FVector PlayerLocation = GetPlayerLocation();
    
    TArray<AANS_ZombieSpawner*> SuitableSpawnPoints; 
    TMap<AANS_ZombieSpawner*, bool> SpawnerHasZombie;
    
    // 현재 좀비 위치 확인
    TArray<AActor*> ExistingZombies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), ExistingZombies);
    
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
    
    // 적합한 스포너 찾기
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
        float DistanceToPlayer = FVector::Dist(PlayerLocation, SpawnerLocation);

        if (DistanceToPlayer < MinSpawnDistance || DistanceToPlayer > MaxSpawnDistance)
        {
            continue;
        }
        
        SuitableSpawnPoints.Add(Spawner);
    }

    if (SuitableSpawnPoints.Num() <= 0)
    {
        return;
    }
    
    // 스포너 목록 무작위 섞기
    Algo::RandomShuffle(SuitableSpawnPoints);
    
    // 1초당 1마리 스폰 제한
    int32 NumZombiesToSpawnThisWave = FMath::Min(1, FMath::Min(Missing, SuitableSpawnPoints.Num()));
    
    if (NumZombiesToSpawnThisWave > 0)
    {
        SpawnZombieAtPoint(SuitableSpawnPoints[0]);
    }
}

void ANS_GameModeBase::SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnSpawner) 
{
	if (!SpawnSpawner) 
	{
		return;
	}

	// 좀비 클래스 선택
	TSubclassOf<ANS_ZombieBase> ZombieToSpawn = nullptr;
	int32 RandNum = FMath::RandRange(1, 100); 
	
	if (RandNum <= 60 && BasicZombieClass) // 60% 로 기본 좀비 생성
	{
		ZombieToSpawn = BasicZombieClass;
	}
	else if (RandNum <= 95 && FatZombieClass) // 35% 로 뚱 좀비 생성
	{
		ZombieToSpawn = FatZombieClass;
	}
	else if (RandNum <= 100 && RunnerZombieClass) 
	{
		ZombieToSpawn = RunnerZombieClass; // 5% 로 여자 좀비 생성
	}

	if (!ZombieToSpawn)
	{
		return; 
	}

	// 스폰 위치 가져오기
	FVector SpawnLocation = SpawnSpawner->GetRandomSpawnLocationInBounds();
	
	// 지면 위에 스폰하기 위한 트레이스 수행
	FHitResult HitResult;
	FVector TraceStart = SpawnLocation + FVector(0, 0, 100); // 약간 위에서 시작
	FVector TraceEnd = SpawnLocation - FVector(0, 0, 500);   // 충분히 아래까지 트레이스
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(SpawnSpawner);
	
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
		SpawnLocation = HitResult.Location + FVector(0, 0, 10); // 지면 위 10 유닛에 위치
	}
	
	FRotator SpawnRotation = FRotator::ZeroRotator; 
	FTransform SpawnTransform(SpawnRotation, SpawnLocation); 

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.bNoFail = true; 
	
	// 좀비 스폰
	ANS_ZombieBase* Zombie = GetWorld()->SpawnActor<ANS_ZombieBase>(ZombieToSpawn, SpawnTransform, Params);
	
	// 좀비 활성화 매니저 찾기
	AActor* ZombieActivationManager = UGameplayStatics::GetActorOfClass(GetWorld(), ANS_ZombieActivationManager::StaticClass());
	ANS_ZombieActivationManager* ZombieActivationManagerCasted = Cast<ANS_ZombieActivationManager>(ZombieActivationManager);
	
	if (Zombie)
	{
		Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed); 
		++CurrentZombieCount; 

		if (ZombieActivationManagerCasted)
		{
			ZombieActivationManagerCasted->AppendSpawnZombie(Zombie);
		}
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