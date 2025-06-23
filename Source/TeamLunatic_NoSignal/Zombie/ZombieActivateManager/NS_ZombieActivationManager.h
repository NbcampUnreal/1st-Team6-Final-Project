#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	float ActivationDistance = 3000.f; // 활성화 반경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Activation")
	float UpdateInterval = 0.5f; // 업데이트 주기 (초)

	UPROPERTY()
	TArray<ANS_ZombieBase*> AllZombiesInLevel; // 레벨의 모든 좀비 배열
	FTimerHandle ActivationUpdateTimerHandle; // 업데이트 타이머 핸들
	UFUNCTION(Server, Reliable)
	void PerformActivationUpdate(); // 실제 활성화/비활성화 로직 함수

	void AppendSpawnZombie(ANS_ZombieBase* Zombie); // 좀비 스폰 함수
};


