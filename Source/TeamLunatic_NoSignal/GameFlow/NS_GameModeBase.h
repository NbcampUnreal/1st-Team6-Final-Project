#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "NS_GameModeBase.generated.h"

UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
	
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const; 
    virtual FVector GetPlayerLocation_Implementation() const; 
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    void OnPlayerCharacterDied(class ANS_PlayerCharacterBase* DeadCharacter);
    virtual void OnPlayerCharacterDied_Implementation(class ANS_PlayerCharacterBase* DeadCharacter) PURE_VIRTUAL(ANS_GameModeBase::OnPlayerCharacterDied, );
    
    // 타이머로 1초마다 현재 좀비를 체크해서 좀비를 스폰하는 함수
    virtual void CheckAndSpawnZombies();

    // 전체 좀비에서 줄어든 만큼 좀비 스폰 함수
    // 스포너에서 좀비를 스폰합니다. 스포너의 스케일을 활용하여 위치를 결정
    virtual void SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnPoint);
    // 좀비 사망시 콜백

    FTimerHandle DelayedSpawnerSearchTimer;

    UFUNCTION()
    void SearchForSpawnersDelayed();
    
    UFUNCTION()
    void OnZombieDestroyed(AActor* DestroyedActor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    // 현재 좀비 수
    int32 CurrentZombieCount = 0;

    // 최대 좀비 수
    UPROPERTY(EditAnywhere, Category = "Zombie")
    int32 MaxZombieCount = 150;
    
    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_BasicZombie> BasicZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_FatZombie> FatZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_RunnerZombie> RunnerZombieClass;

    // 레벨에 있는 모든 좀비 스포너를 저장할 배열
    TArray<AANS_ZombieSpawner*> ZombieSpawnPoints;

    // 좀비 스폰 타이머 핸들
    FTimerHandle ZombieSpawnTimer;

    // -- 스폰 거리 관련 변수 --
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float MinSpawnDistance = 3000.0f; // 플레이어로부터 좀비가 스폰될 수 있는 최소 거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    float MaxSpawnDistance = 6000.0f; // 플레이어로부터 좀비가 스폰될 수 있는 최대 거리

};