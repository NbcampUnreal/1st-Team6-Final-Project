#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "NavigationInvokerComponent.h"
#include "NS_Chaser.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_Chaser : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_Chaser();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Invoker 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UNavigationInvokerComponent* NavigationInvoker;

protected:
	virtual void BeginPlay() override;

	// 에디터에서 자동 데미지 테스트를 켜고 끌 수 있는 변수
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bEnableAutoDamageTest;

	// 10초 뒤 복구를 처리할 함수
	void RecoverFromKneel();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Stat")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Stat")
	float CurrentHealth;

private:
	// 타이머들을 제어하기 위한 핸들
	FTimerHandle KneelRecoveryTimerHandle;
	FTimerHandle AutoDamageTimerHandle;

	// 주기적으로 데미지를 입히는 함수
	void ApplyAutoDamage();

};