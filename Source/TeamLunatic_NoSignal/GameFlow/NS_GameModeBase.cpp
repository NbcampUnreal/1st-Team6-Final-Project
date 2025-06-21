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
#include "DrawDebugHelpers.h" // 디버그 드로잉을 위해 추가
// #include "Kismet/KismetSystemLibrary.h" // UE_LOG 사용 시 필요 (이미 위에서 include되었을 수 있음)


void ANS_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AANS_ZombieSpawner::StaticClass(), FoundActors); 
	
    ZombieSpawnPoints.Empty(); 
    for (AActor* Actor : FoundActors)
    {
        if (AANS_ZombieSpawner* Spawner = Cast<AANS_ZombieSpawner>(Actor))
        {
            ZombieSpawnPoints.Add(Spawner);
        }
    }
	// UE_LOG(LogTemp, Warning, TEXT("Found %d Zombie Spawners in BeginPlay."), ZombieSpawnPoints.Num()); // 디버그 로그 추가
	
	TArray<AActor*> ExistingZombies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	  ANS_ZombieBase::StaticClass(), ExistingZombies);

	CurrentZombieCount = ExistingZombies.Num();
	
	for (AActor* Z : ExistingZombies)
	{
		if (IsValid(Z))
		{
			Z->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
		}
	}

	GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this,
	  &ANS_GameModeBase::CheckAndSpawnZombies, 10.0f, true);
}

FVector ANS_GameModeBase::GetPlayerLocation_Implementation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void ANS_GameModeBase::CheckAndSpawnZombies()
{
    int32 Missing = MaxZombieCount - CurrentZombieCount;
    // UE_LOG(LogTemp, Warning, TEXT("CheckAndSpawnZombies called. Missing: %d, CurrentZombieCount: %d"), Missing, CurrentZombieCount);
    
    // 이전에 플레이어 위치 반경 3000 유닛 내 스포너만 필터링하는 로직이 있었으나,
    // 일단은 모든 활성화된 스포너를 대상으로 스폰을 시도하도록 임시 변경 (테스트용)
    // 실제 게임에서는 3000 유닛 범위 로직을 다시 추가해야 함
    TArray<AANS_ZombieSpawner*> ActiveAndValidSpawnPoints; // 유효한 스포너 목록
    FVector PlayerLocation = GetPlayerLocation(); 

    for (AANS_ZombieSpawner* Spawner : ZombieSpawnPoints) 
    {
        if (IsValid(Spawner) && Spawner->bIsEnabled)
        {
            float Distance = FVector::Dist(PlayerLocation, Spawner->GetActorLocation());
            if (Distance <= 3000.0f) // 플레이어 주변 3000 유닛 범위 내 스포너만 고려
            {
                ActiveAndValidSpawnPoints.Add(Spawner);
            }
        }
    }
    // UE_LOG(LogTemp, Warning, TEXT("Active and Valid SpawnPoints found: %d"), ActiveAndValidSpawnPoints.Num());

    // 스폰해야 할 총 좀비 수 (최대 15마리까지, 5마리씩 스폰)
    // 예시: 5마리를 스폰하고 싶다면 TotalZombiesToSpawn = FMath::Min(Missing, 5); 로 설정
    // 현재 요구사항에 따라 5마리씩 스폰한다면 Missing에서 5를 넘지 않게 조정
    int32 NumZombiesToSpawnThisWave = FMath::Min(Missing, 10); // 10초마다 최대 5마리 스폰

    // 스폰해야 할 좀비가 있고, 유효한 스포너가 1개 이상 있을 경우
    if (NumZombiesToSpawnThisWave > 0 && ActiveAndValidSpawnPoints.Num() > 0) 
    {
        // 스포너를 무작위로 섞음
        Algo::RandomShuffle(ActiveAndValidSpawnPoints);

        int32 CurrentSpawnedCount = 0;
        int32 SpawnerIndex = 0;

        while (CurrentSpawnedCount < NumZombiesToSpawnThisWave)
        {
            // 모든 스포너를 순환
            AANS_ZombieSpawner* SpawnerToUse = ActiveAndValidSpawnPoints[SpawnerIndex % ActiveAndValidSpawnPoints.Num()];
            
            // 이 스포너에서 좀비 한 마리 스폰
            SpawnZombieAtPoint(SpawnerToUse);
            CurrentSpawnedCount++;

            // 다음 스포너로 이동 (Round Robin)
            SpawnerIndex++;
        }
        // UE_LOG(LogTemp, Warning, TEXT("Attempted to spawn %d zombies this wave."), CurrentSpawnedCount);
    }
}

void ANS_GameModeBase::SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnSpawner) 
{
    if (!SpawnSpawner) 
    {
        // UE_LOG(LogTemp, Error, TEXT("SpawnZombieAtPoint: SpawnSpawner is null."));
        return;
    }

    TSubclassOf<ANS_ZombieBase> ZombieToSpawn = nullptr;
    int32 RandNum = FMath::RandRange(1, 100); 

    if (RandNum <= 40 && BasicZombieClass) 
    {
        ZombieToSpawn = BasicZombieClass;
    }
    else if (RandNum <= 70 && FatZombieClass) 
    {
        ZombieToSpawn = FatZombieClass;
    }
    else if (RandNum <= 100 && RunnerZombieClass) 
    {
        ZombieToSpawn = RunnerZombieClass;
    }

    if (!ZombieToSpawn)
    {
        // UE_LOG(LogTemp, Error, TEXT("SpawnZombieAtPoint: No zombie class selected for spawn."));
        return; 
    }

    // 스포너 액터의 GetRandomSpawnLocationInBounds 함수를 호출하여 랜덤 스폰 위치 획득
    // 이 함수가 스포너의 스케일을 고려한 랜덤 위치를 반환합니다.
    FVector SpawnLocation = SpawnSpawner->GetRandomSpawnLocationInBounds(); 
    FRotator SpawnRotation = FRotator::ZeroRotator; 
    // 좀비가 스폰될 때 약간 랜덤한 회전을 주는 것도 자연스러움을 더할 수 있습니다.
    // FRotator SpawnRotation = FRotator(0, FMath::RandRange(0, 360), 0); 
    FTransform SpawnTransform(SpawnRotation, SpawnLocation); 

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    Params.bNoFail = true; 
	
    ANS_ZombieBase* Zombie = GetWorld()->SpawnActor<ANS_ZombieBase>(ZombieToSpawn, SpawnTransform, Params);
	AActor* ZombieActivationManager = UGameplayStatics::GetActorOfClass(GetWorld(),ANS_ZombieActivationManager::StaticClass());
	ANS_ZombieActivationManager* ZombieActivationManagerCasted = Cast<ANS_ZombieActivationManager>(ZombieActivationManager);
	
    if (Zombie)
    {
        Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed); 
        ++CurrentZombieCount; 

    	if (ZombieActivationManagerCasted)
    	{
    		ZombieActivationManagerCasted->AppendSpawnZombie(Zombie);
    	}
        // UE_LOG(LogTemp, Warning, TEXT("Zombie spawned successfully at %s by spawner %s."), *Zombie->GetActorLocation().ToString(), *SpawnSpawner->GetName());
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("Failed to spawn zombie at: %s from spawner %s."), *SpawnLocation.ToString(), *SpawnSpawner->GetName());
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