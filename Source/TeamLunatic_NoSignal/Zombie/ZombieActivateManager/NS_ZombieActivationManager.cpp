#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/NS_PlayerCharacterBase.h" 
#include "Zombie/NS_ZombieBase.h" // 이 경로가 정확해야 합니다.
#include "AIController.h"

ANS_ZombieActivationManager::ANS_ZombieActivationManager()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true); // 이 매니저는 서버에서만 동작하는 것이 효율적입니다.
    PrimaryActorTick.bStartWithTickEnabled = true; // BeginPlay 전에 활성화
}

void ANS_ZombieActivationManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority()) 
    {
        // === 변경된 부분 시작 ===
        TArray<AActor*> FoundActors; // AActor* 타입의 임시 배열 선언
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), FoundActors);

        // FoundActors에 있는 각 액터를 ANS_ZombieBase*로 캐스팅하여 AllZombiesInLevel에 추가
        for (AActor* Actor : FoundActors)
        {
            if (ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(Actor))
            {
                AllZombiesInLevel.Add(Zombie);
            }
        }
        // === 변경된 부분 끝 ===
        
        GetWorldTimerManager().SetTimer(ActivationUpdateTimerHandle, this, &ANS_ZombieActivationManager::PerformActivationUpdate, UpdateInterval, true);
    }
    else
    {
        PrimaryActorTick.bCanEverTick = false;
    }
}

void ANS_ZombieActivationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // PerformActivationUpdate가 타이머로 호출되므로 여기서는 직접적인 로직 없음
}

void ANS_ZombieActivationManager::PerformActivationUpdate_Implementation()
{
    if (!HasAuthority()) return; // 서버에서만 실행

    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_PlayerCharacterBase::StaticClass(), Players);

    if (Players.Num() == 0) return;

    for (ANS_ZombieBase* Zombie : AllZombiesInLevel)
    {
        if (!IsValid(Zombie) || Zombie->bIsDead)
        {
            continue;
        }

        bool bShouldBeActive = false;
        FVector ZombieLocation = Zombie->GetActorLocation();

        for (AActor* PlayerActor : Players)
        {
            ANS_PlayerCharacterBase* PlayerChar = Cast<ANS_PlayerCharacterBase>(PlayerActor);
            if (IsValid(PlayerChar) && !PlayerChar->IsDead)
            {
                float DistanceSq = FVector::DistSquared(ZombieLocation, PlayerChar->GetActorLocation());
                if (DistanceSq <= FMath::Square(ActivationDistance))
                {
                    bShouldBeActive = true;
                    break; 
                }
            }
        }

        if (bShouldBeActive)
        {
            if (!Zombie->bIsActive) // 현재 비활성 상태인데 활성화되어야 한다면
            {
                Zombie->SetActive_Multicast(true); // 이 줄이 OnRep_IsActive를 트리거합니다.
                // 서버에서는 AI 관련 로직 즉시 활성화
                Zombie->SetActorTickEnabled(true); 
                if (AAIController* AIController = Cast<AAIController>(Zombie->GetController()))
                {
                    AIController->SetActorTickEnabled(true);
                    // AIController->ResumeBehaviorTree(); // 필요한 경우
                }
            }
        }
        else // 비활성화되어야 한다면
        {
            if (Zombie->bIsActive) // 현재 활성 상태인데 비활성화되어야 한다면
            {
                Zombie->SetActive_Multicast(false); // 이 줄이 OnRep_IsActive를 트리거합니다.
                // 서버에서는 AI 관련 로직 즉시 비활성화
                Zombie->SetActorTickEnabled(false);
                if (AAIController* AIController = Cast<AAIController>(Zombie->GetController()))
                {
                    AIController->SetActorTickEnabled(false);
                    // AIController->PauseBehaviorTree(); // 필요한 경우
                }
            }
        }
    }
}

void ANS_ZombieActivationManager::AppendSpawnZombie(ANS_ZombieBase* Zombie)
{
    AllZombiesInLevel.Add(Zombie);
}
