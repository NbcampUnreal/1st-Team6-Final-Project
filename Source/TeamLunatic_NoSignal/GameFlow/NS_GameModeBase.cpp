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

	// 레벨의 모든 좀비 스포너 찾기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AANS_ZombieSpawner::StaticClass(), FoundActors); 
	
	ZombieSpawnPoints.Empty(); 
	
	for (AActor* Actor : FoundActors)
	{
		if (AANS_ZombieSpawner* Spawner = Cast<AANS_ZombieSpawner>(Actor))
		{
			if (Spawner->GetLevel() == GetLevel())
			{
				ZombieSpawnPoints.Add(Spawner);
			}
		}
	}
	
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

	// 좀비 스폰 타이머 설정 - 이 부분은 자식 클래스에서 설정하도록 주석 처리
	// GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this, &ANS_GameModeBase::CheckAndSpawnZombies, 1.0f, true);
	
	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] BeginPlay 완료. 좀비 스포너 수: %d, 기존 좀비 수: %d"), 
		ZombieSpawnPoints.Num(), CurrentZombieCount);
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
	int32 Missing = MaxZombieCount - CurrentZombieCount;
	FVector PlayerLocation = GetPlayerLocation(); 
	
	if (Missing <= 0)
	{
		return;
	}
	
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

/**
 * SpawnZombieAtPoint - 지정된 스포너에서 좀비를 스폰하는 함수
 */
void ANS_GameModeBase::SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnSpawner) 
{
	// 스포너가 유효한지 확인합니다
	if (!SpawnSpawner) 
	{
		return;
	}

	// 좀비 클래스 선택
	TSubclassOf<ANS_ZombieBase> ZombieToSpawn = nullptr;
	int32 RandNum = FMath::RandRange(1, 100); 

	FString ZombieTypeName = TEXT("알 수 없음");
	
	if (RandNum <= 60 && BasicZombieClass) // 60% 로 기본 좀비 생성
	{
		ZombieToSpawn = BasicZombieClass;
		ZombieTypeName = TEXT("기본 좀비");
	}
	else if (RandNum <= 95 && FatZombieClass) // 35% 로 뚱 좀비 생성
	{
		ZombieToSpawn = FatZombieClass;
		ZombieTypeName = TEXT("뚱 좀비");
	}
	else if (RandNum <= 100 && RunnerZombieClass) 
	{
		ZombieToSpawn = RunnerZombieClass; // 5% 로 여자 좀비 생성
		ZombieTypeName = TEXT("러너 좀비");
	}

	if (!ZombieToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("스폰할 좀비 클래스가 없습니다. 랜덤 값: %d"), RandNum);
		return; 
	}

	// 스폰 위치 및 파라미터 설정
	FVector SpawnLocation = SpawnSpawner->GetRandomSpawnLocationInBounds(); 
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
		UE_LOG(LogTemp, Warning, TEXT("[좀비 스폰 성공] ID: %s, 현재 좀비 수: %d"), *Zombie->GetName(), CurrentZombieCount + 1);
		
		Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed); 
		++CurrentZombieCount; 

		if (ZombieActivationManagerCasted)
		{
			ZombieActivationManagerCasted->AppendSpawnZombie(Zombie);
		}
	}
	else
	{
		// 플레이어 위치 가져오기
		FVector PlayerLocation = GetPlayerLocation();
		float DistanceToPlayer = FVector::Dist(PlayerLocation, SpawnLocation);
		
		// 스폰 실패 원인 분석
		FString FailReason;
		if (DistanceToPlayer < MinSpawnDistance)
		{
			FailReason = FString::Printf(TEXT("플레이어와 너무 가까움 (거리: %.1f, 최소 거리: %.1f)"), 
				DistanceToPlayer, MinSpawnDistance);
		}
		else if (DistanceToPlayer > MaxSpawnDistance)
		{
			FailReason = FString::Printf(TEXT("플레이어와 너무 멀리 떨어짐 (거리: %.1f, 최대 거리: %.1f)"), 
				DistanceToPlayer, MaxSpawnDistance);
		}
		else
		{
			FailReason = TEXT("충돌 또는 기타 이유");
		}
		
		UE_LOG(LogTemp, Error, TEXT("[좀비 스폰 실패] 위치: X=%.1f Y=%.1f Z=%.1f, 플레이어와의 거리: %.1f, 원인: %s"), 
			SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, DistanceToPlayer, *FailReason);
	}
}

/**
 * OnZombieDestroyed - 좀비가 파괴될 때 호출되는 콜백 함수
 */
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