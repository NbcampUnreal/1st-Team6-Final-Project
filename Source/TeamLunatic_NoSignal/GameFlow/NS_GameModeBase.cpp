#include "GameFlow/NS_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h" // Algo::RandomShuffle을 사용하므로 이 헤더를 사용합니다.
#include "Zombie/Zombies/NS_BasicZombie.h" // BasicZombie 헤더 추가
#include "Zombie/Zombies/NS_FatZombie.h"   // FatZombie 헤더 추가
#include "Zombie/Zombies/NS_RunnerZombie.h" // RunnerZombie 헤더 추가
#include "GameFramework/Actor.h" // AActor를 위해 필요할 수 있음
#include "Engine/World.h" // GetWorld() 사용을 위해 필요
#include "TimerManager.h" // FTimerManager 사용을 위해 필요
#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"

void ANS_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 스폰포인트 
	// GetAllActorsOfClass는 레벨에 로드된 모든 액터를 가져오고
	// BeginPlay 시점에 모든 스폰 포인트가 로드되어 있는지 확인해야 함
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	  AActor::StaticClass(), SpawnPoints);

	// "ZombieSpawnPoint" 태그를 가진 액터를 월드에 배치하면 거기서 좀비 스폰
	SpawnPoints.RemoveAll([](AActor* Actor)
	{
	  return !Actor->ActorHasTag("ZombieSpawnPoint");
	});
	

	// 시작 시 레벨에 배치된 좀비 수 측정
	TArray<AActor*> ExistingZombies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	  ANS_ZombieBase::StaticClass(), ExistingZombies);

	CurrentZombieCount = ExistingZombies.Num();
	
	// 좀비의 사망 감지용 이미 존재하는 좀비들에 대해
	for (AActor* Z : ExistingZombies)
	{
		if (IsValid(Z))
		{
			Z->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
		}
	}

	// 타이머 시작 10초마다 체크
	GetWorldTimerManager().SetTimer(ZombieSpawnTimer, this,
	  &ANS_GameModeBase::CheckAndSpawnZombies, 10.0f, true);
}

void ANS_GameModeBase::CheckAndSpawnZombies()
{
	int32 Missing = MaxZombieCount - CurrentZombieCount;
	
	// Missing이 5 이상이고 스폰 포인트가 5개 이상일 경우 스폰 진행
	if (Missing >= 5 && SpawnPoints.Num() >= 5)
	{
		// 랜덤하게 5개의 스폰 포인트 선택
		TArray<AActor*> Shuffled = SpawnPoints;
		Algo::RandomShuffle(Shuffled); // Algo::Shuffle 대신 Algo::RandomShuffle을 사용합니다.

        int32 NumToSpawn = FMath::Min(Missing, 20); // 부족한 좀비 수와 5 중 더 작은 값만큼 스폰 (최대 5마리)
        NumToSpawn = FMath::Min(NumToSpawn, Shuffled.Num()); // 스폰 포인트 수보다 많이 스폰하지 않도록
		
		for (int32 i = 0; i < NumToSpawn; ++i)
		{
			SpawnZombieAtPoint(Shuffled[i]);
		}
	}
}

void ANS_GameModeBase::SpawnZombieAtPoint(AActor* SpawnPoint)
{
	// 스폰 포인트 없으면 리턴
    if (!SpawnPoint)
    {
        return;
    }

    TSubclassOf<ANS_ZombieBase> ZombieToSpawn = nullptr;
    int32 RandNum = FMath::RandRange(1, 100); // 1부터 100까지의 랜덤 숫자 생성

    // 확률에 따라 좀비 클래스 선택
    if (RandNum <= 40 && BasicZombieClass) // 40% 확률로 BasicZombie 
    {
        ZombieToSpawn = BasicZombieClass;
    }
    else if (RandNum <= 70 && FatZombieClass) // 30% 확률로 FatZombie
    {
        ZombieToSpawn = FatZombieClass;
    }
    else if (RandNum <= 100 && RunnerZombieClass) // 30% 확률로 RunnerZombie
    {
        ZombieToSpawn = RunnerZombieClass;
    }

	// 스폰할 좀비 클래스가 없으면 리턴
    if (!ZombieToSpawn)
    {
        return; 
    }

    FTransform SpawnTransform = SpawnPoint->GetActorTransform();

    FActorSpawnParameters Params;
    // 충돌 처리 방식 설정 가능한 경우 조정하고, 불가능하면 스폰 강행하는데 이러면 벽 안에도 스폰될 수 있음
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    Params.bNoFail = true; // 스폰 실패를 강제하지 않지만 그래도 스폰이 안 될 수 있음
	
    ANS_ZombieBase* Zombie = GetWorld()->SpawnActor<ANS_ZombieBase>(ZombieToSpawn, SpawnTransform, Params);
	AActor* ZombieActivationManager = UGameplayStatics::GetActorOfClass(GetWorld(),ANS_ZombieActivationManager::StaticClass());
	ANS_ZombieActivationManager* ZombieActivationManagerCasted = Cast<ANS_ZombieActivationManager>(ZombieActivationManager);
	
    if (Zombie)
    {
        // 새로 스폰된 좀비의 OnDestroyed 델리게이트에 바인딩
        Zombie->OnDestroyed.AddDynamic(this, &ANS_GameModeBase::OnZombieDestroyed);
        ++CurrentZombieCount;

    	if (ZombieActivationManagerCasted)
    	{
    		// 좀비 스폰 매니저에 좀비 추가
    		ZombieActivationManagerCasted->AppendSpawnZombie(Zombie);
    	}
    }
}

void ANS_GameModeBase::OnZombieDestroyed(AActor* DestroyedActor)
{
    // 유효한 좀비 액터인지 다시 한번 확인
    if (IsValid(DestroyedActor))
    {
        // 좀비 액터만 카운트 감소
        if (DestroyedActor->IsA(ANS_ZombieBase::StaticClass()))
        {
            --CurrentZombieCount;
            CurrentZombieCount = FMath::Max(0, CurrentZombieCount); // 0 미만으로 내려가지 않도록 보정
        }
    }
}