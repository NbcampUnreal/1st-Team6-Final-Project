#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "ZOmbie/NS_ZombieBase.h"
#include "NS_GameModeBase.generated.h"

UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANS_GameModeBase();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const; 
    virtual FVector GetPlayerLocation_Implementation() const; 

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    void OnPlayerCharacterDied(class ANS_PlayerCharacterBase* DeadCharacter);
    virtual void OnPlayerCharacterDied_Implementation(class ANS_PlayerCharacterBase* DeadCharacter);
    // 캐시된 좀비 리스트 (약한 참조 사용)

protected:

    virtual void BeginPlay() override;
    // 타이머로 주기적으로 현재 좀비를 체크해서 좀비를 스폰하는 함수
    virtual void CheckAndSpawnZombies();

    // 전체 좀비에서 줄어든 만큼 좀비 스폰 함수
    virtual void SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnPoint);

    // 플레이어로부터 너무 멀리 있는 좀비 제거 함수
    UFUNCTION()
    virtual void CleanupDistantZombies();


    // 좀비 스폰 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float ZombieSpawnInterval = 1;
    
    // 한 번에 스폰할 좀비 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    int32 ZombiesPerSpawn = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    // 현재 좀비 수
    int32 CurrentZombieCount = 0;

    // 최대 좀비 수
    UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category = "Zombie")
    int32 MaxZombieCount = 50;
    
    // 플레이어 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    int32 PlayerCount = 1;
    
    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_BasicZombie> BasicZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_FatZombie> FatZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_RunnerZombie> RunnerZombieClass;

    // 레벨에 있는 모든 좀비 스포너를 저장할 배열
    UPROPERTY()
    TArray<AANS_ZombieSpawner*> ZombieSpawnPoints;

    // 좀비 스폰 타이머 핸들
    FTimerHandle ZombieSpawnTimer;

    // 스폰 거리 관련 변수 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float MinSpawnDistance = 3000.0f; // 플레이어로부터 좀비가 스폰될 수 있는 최소 거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float MaxSpawnDistance = 6000.0f; // 플레이어로부터 좀비가 스폰될 수 있는 최대 거리;

    // 좀비 제거 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float ZombieDestroyDistance = 5001.0f;
    
    // 타이머 핸들
    FTimerHandle ZombieCleanupTimer;

    // 좀비 사망시 콜백
    FTimerHandle DelayedSpawnerSearchTimer;

    // 디버그 카운터 (디버그 타이머는 제거)
    int32 ZombiesInCloseRange = 0;    // 4000 이내
    int32 ZombiesInMidRange = 0;      // 4000-8000 사이
    int32 ZombiesRemoved = 0;         // 제거된 좀비 수

    // 강화된 캐싱 시스템 변수들
    UPROPERTY()
    TArray<TWeakObjectPtr<ANS_ZombieBase>> CachedZombies;

    UPROPERTY()
    TArray<TWeakObjectPtr<ANS_PlayerCharacterBase>> CachedPlayers;

    UPROPERTY()
    TArray<TWeakObjectPtr<AANS_ZombieSpawner>> CachedSpawners;

    // 마지막 업데이트 시간들
    float LastZombieListUpdateTime = 0.0f;
    float LastPlayerListUpdateTime = 0.0f;
    float LastSpawnerListUpdateTime = 0.0f;

    // 업데이트 간격 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Caching")
    float ZombieListUpdateInterval = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Caching")
    float PlayerListUpdateInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Caching")
    float SpawnerListUpdateInterval = 30.0f;
    

    UFUNCTION()
    void SearchForSpawnersDelayed();
    
    UFUNCTION()
    void OnZombieDestroyed(AActor* DestroyedActor);

    // 적합한 스포너 찾기
    TArray<AANS_ZombieSpawner*> FindSuitableSpawners(const FVector& PlayerLocation);
    
    // 좀비 클래스 선택 함수
    TSubclassOf<ANS_ZombieBase> SelectZombieClass();
    
    // 지면 위 스폰 위치 계산 함수
    FVector GetSpawnLocationOnGround(AANS_ZombieSpawner* Spawner);
    
    // 좀비 스폰 함수
    ANS_ZombieBase* SpawnZombieAtLocation(TSubclassOf<ANS_ZombieBase> ZombieClass, const FVector& Location);
    
    // 스폰된 좀비 등록 함수
    void RegisterSpawnedZombie(ANS_ZombieBase* Zombie);
    
    // 최적화된 스폰 체크 함수
    UFUNCTION()
    void OptimizedSpawnCheck();

    // 배치 처리된 좀비 정리 함수
    UFUNCTION()
    void BatchedCleanup();

    // 타이머 주기 계산 함수들
    float CalculateOptimizedSpawnInterval();
    float CalculateOptimizedCleanupInterval();

    // 강화된 캐싱 함수들
    TArray<ANS_ZombieBase*> GetCachedZombies();
    TArray<ANS_PlayerCharacterBase*> GetCachedPlayers();
    TArray<AANS_ZombieSpawner*> GetCachedSpawners();

    // 캐시 업데이트 함수들
    void UpdateZombieCache();
    void UpdatePlayerCache();
    void UpdateSpawnerCache();

    // 거리 계산 최적화 함수들
    bool IsWithinDistanceSquared(const FVector& Location1, const FVector& Location2, float DistanceThreshold);
    TArray<ANS_ZombieBase*> GetZombiesInRange(const FVector& CenterLocation, float Range);

    // 캐시된 좀비 리스트 반환 함수
    TArray<ANS_ZombieBase*> GetValidZombies();
};
