#pragma once

#include "CoreMinimal.h"
#include "Zombie/NS_ZombieBase.h"
#include "NavigationInvokerComponent.h"  // 네비게이션 인보커 컴포넌트 헤더 추가
#include "NS_Chaser.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_Chaser : public ANS_ZombieBase
{
	GENERATED_BODY()

public:
	ANS_Chaser();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 체이서 좀비는 항상 활성화 상태 유지하기 위해 오버라이드
	virtual void SetActive_Multicast_Implementation(bool setActive) override;

protected:
	virtual void BeginPlay() override;
	

	// 에디터에서 자동 데미지 테스트를 켜고 끌 수 있는 변수
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bEnableAutoDamageTest;

	// 10초 뒤 복구를 처리할 함수
	void RecoverFromKneel();


private:
	// 타이머들을 제어하기 위한 핸들
	FTimerHandle KneelRecoveryTimerHandle;
	FTimerHandle AutoDamageTimerHandle;
	
	// 주기적으로 데미지를 입히는 함수
	void ApplyAutoDamage();

public:
	virtual void Tick(float DeltaTime) override;
};