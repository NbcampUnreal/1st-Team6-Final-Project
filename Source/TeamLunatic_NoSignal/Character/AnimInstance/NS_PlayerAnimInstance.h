#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NS_PlayerAnimInstance.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class ANS_PlayerCharacterBase* PlayerCharacter;
	
	// 최종 애니메이션에 바인딩할 보간된 Aim 값
	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	float AimPitch;

	// 보간 속도 (에디터에서 조정 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim")
	float AimInterpSpeed = 15.f;
	
	// ======== Turn In Place 관련 변수들 ========
	UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
	bool bIsTurningLeft = false;

	UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
	bool bIsTurningRight = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	float TurnInPlaceAngleThreshold = 45.0f; // Turn In Place 발동 임계각

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TurnInPlace")
	float TurnInPlaceInterpSpeed = 5.0f; // Turn In Place 회전 속도

	// 캐릭터의 bUseControllerRotationYaw를 제어하는 함수
	// Character에서 호출될 수 있도록 UFUNCTION으로 선언
	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetCharacterUseControllerRotationYaw(bool bNewValue);
};
