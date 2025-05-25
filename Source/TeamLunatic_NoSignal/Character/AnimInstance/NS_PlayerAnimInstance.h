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
	// 소유 캐릭터
	UPROPERTY(BlueprintReadOnly, Category="Character")
	class ANS_PlayerCharacterBase* PlayerCharacter = nullptr;

	// AimOffset 보간값
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimYaw   = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch = 0.f;

	// TurnInPlace 상태
	UPROPERTY(BlueprintReadOnly, Category="TurnInPlace")
	bool bIsTurningInPlace = false;
	UPROPERTY(BlueprintReadOnly, Category="TurnInPlace")
	bool bIsTurningLeft = false;
	UPROPERTY(BlueprintReadOnly, Category="TurnInPlace")
	bool bIsTurningRight = false;

	// 시작·종료 임계각
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TurnInPlace")
	float TurnInPlaceActivationAngle = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TurnInPlace")
	float TurnInPlaceDeactivationThreshold = 2.f;

	// 내부 로직
	void UpdateAimOffset(float DeltaSeconds);
	void TurnInPlace(float DeltaSeconds);
};
