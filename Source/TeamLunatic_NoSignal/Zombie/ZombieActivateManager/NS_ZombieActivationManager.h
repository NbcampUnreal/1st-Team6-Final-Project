#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "NS_ZombieActivationManager.generated.h"

class ANS_ZombieBase; // 좀비 기본 클래스 Forward Declaration

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ZombieActivationManager : public AActor
{
	GENERATED_BODY()

public:	
	ANS_ZombieActivationManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Activation")
	float ActivationDistance = 4000.f; // 활성화 반경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Activation")
	float UpdateInterval = 2.0f; // 업데이트 주기 (초) - 최적화를 위해 증가

	UPROPERTY()
	TArray<ANS_ZombieBase*> AllZombiesInLevel; // 레벨의 모든 좀비 배열

	UPROPERTY()
	TArray<TWeakObjectPtr<ANS_PlayerCharacterBase>> CachedPlayers; // 캐시된 플레이어 배열

	FTimerHandle ActivationUpdateTimerHandle; // 업데이트 타이머 핸들

	// 캐시 관련 변수들
	float LastPlayerCacheUpdateTime = 0.0f;
	float PlayerCacheUpdateInterval = 3.0f;
	UFUNCTION(Server, Reliable)
	void PerformActivationUpdate(); // 실제 활성화/비활성화 로직 함수

	void AppendSpawnZombie(ANS_ZombieBase* Zombie); // 좀비 스폰 함수

	// 최적화된 플레이어 캐시 함수들
	TArray<ANS_PlayerCharacterBase*> GetCachedPlayers();
	void UpdatePlayerCache();
};


