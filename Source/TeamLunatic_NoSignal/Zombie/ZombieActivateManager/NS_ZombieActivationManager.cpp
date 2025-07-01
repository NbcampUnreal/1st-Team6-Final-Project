#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/NS_PlayerCharacterBase.h" 
#include "Zombie/NS_ZombieBase.h" 
#include "AIController.h"
#include "Zombie/Zombies/NS_Chaser.h" 

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
        // 레벨의 모든 좀비 찾기
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANS_ZombieBase::StaticClass(), FoundActors);

        // 체이서 좀비 찾기 및 즉시 활성화
        for (AActor* Actor : FoundActors)
        {
            ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(Actor);
            if (!Zombie) continue;
            
            AllZombiesInLevel.Add(Zombie);
            
            // 체이서 좀비는 즉시 활성화
            if (Cast<ANS_Chaser>(Zombie))
            {
                Zombie->bIsActive=true;
                if (AAIController* AIController = Cast<AAIController>(Zombie->GetController()))
                {
                    AIController->SetActorTickEnabled(true);
                }
            }
        }
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
    if (!HasAuthority()) return;

    // 캐시된 플레이어 리스트 사용 (GetAllActorsOfClass 대신)
    TArray<ANS_PlayerCharacterBase*> Players = GetCachedPlayers();

    if (Players.Num() == 0) return;

    for (ANS_ZombieBase* Zombie : AllZombiesInLevel)
    {
        if (!IsValid(Zombie) || Zombie->bIsDead)
        {
            continue;
        }

        // 체이서 좀비는 항상 활성화 상태 유지
        if (Cast<ANS_Chaser>(Zombie))
        {
            if (!Zombie->bIsActive)
            {
                Zombie->SetActive(true);
                if (AAIController* AIController = Cast<AAIController>(Zombie->GetController()))
                {
                    AIController->SetActorTickEnabled(true);
                }
            }
            continue; // 체이서 좀비는 추가 처리 필요 없음
        }

        // 일반 좀비 활성화 로직
        bool bShouldBeActive = false;
        FVector ZombieLocation = Zombie->GetActorLocation();
        
        // 최적화된 플레이어 거리 체크 (캐스팅 불필요)
        float ActivationDistanceSquared = ActivationDistance * ActivationDistance;
        for (ANS_PlayerCharacterBase* PlayerChar : Players)
        {
            if (IsValid(PlayerChar) && !PlayerChar->IsDead)
            {
                float DistanceSq = FVector::DistSquared(ZombieLocation, PlayerChar->GetActorLocation());
                if (DistanceSq <= ActivationDistanceSquared)
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
                Zombie->SetActive(true);

            }
        }
        else // 비활성화되어야 한다면
        {
            if (Zombie->bIsActive) // 현재 활성 상태인데 비활성화되어야 한다면
            {
                Zombie->SetActive(false);
            }
        }
    }
}

void ANS_ZombieActivationManager::AppendSpawnZombie(ANS_ZombieBase* Zombie)
{
    AllZombiesInLevel.Add(Zombie);
}

// 최적화된 플레이어 캐시 함수들
TArray<ANS_PlayerCharacterBase*> ANS_ZombieActivationManager::GetCachedPlayers()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 캐시 업데이트가 필요한지 확인
    if (CurrentTime - LastPlayerCacheUpdateTime > PlayerCacheUpdateInterval)
    {
        UpdatePlayerCache();
        LastPlayerCacheUpdateTime = CurrentTime;
    }

    // 유효한 플레이어들만 반환
    TArray<ANS_PlayerCharacterBase*> ValidPlayers;
    ValidPlayers.Reserve(CachedPlayers.Num());

    for (int32 i = CachedPlayers.Num() - 1; i >= 0; --i)
    {
        if (CachedPlayers[i].IsValid())
        {
            ValidPlayers.Add(CachedPlayers[i].Get());
        }
        else
        {
            // 무효한 포인터 제거
            CachedPlayers.RemoveAtSwap(i);
        }
    }

    return ValidPlayers;
}

void ANS_ZombieActivationManager::UpdatePlayerCache()
{
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
}


