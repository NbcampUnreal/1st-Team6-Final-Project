// NS_GameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Zombie/NS_ZombieBase.h"
#include "NS_GameModeBase.generated.h"

// NS_GameModeBase.h
UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

protected:
  virtual void BeginPlay() override;
	
public:
    // 플레이어 위치 반환 
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const;
    virtual FVector GetPlayerLocation_Implementation() const PURE_VIRTUAL(ANS_GameModeBase::GetPlayerLocation, return FVector::ZeroVector;);

    // 사망 알림 함수 
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    void OnPlayerCharacterDied(class ANS_PlayerCharacterBase* DeadCharacter);
    virtual void OnPlayerCharacterDied_Implementation(class ANS_PlayerCharacterBase* DeadCharacter) PURE_VIRTUAL(ANS_GameModeBase::OnPlayerCharacterDied, );
    
    
    // 타이머로 10초마다 현재 좀비를 체크해서 좀비를 스폰하는 함수
    void CheckAndSpawnZombies();

    // 전체 좀비에서 줄어든 만큼 좀비 스폰 함수
    void SpawnZombieAtPoint(AActor* SpawnPoint);

    // 좀비 사망시 콜백
    UFUNCTION()
    void OnZombieDestroyed(AActor* DestroyedActor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    // 현재 좀비 수
    int32 CurrentZombieCount = 0;

    // 최대 좀비 수
    UPROPERTY(EditAnywhere, Category = "Zombie")
    int32 MaxZombieCount = 10;
    
    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_BasicZombie> BasicZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_FatZombie> FatZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_RunnerZombie> RunnerZombieClass;

    // 스폰 포인트 목록
    TArray<AActor*> SpawnPoints;

    FTimerHandle ZombieSpawnTimer;
};
