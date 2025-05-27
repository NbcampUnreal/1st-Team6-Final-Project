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

	// PlayerCharacterBase에 있는 CamYaw값과는 다른 로컬용 Yaw값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aim")
	float AimYaw   = 0.f;
	// PlayerCharacterBase에 있는 CamPitch값과는 다른 로컬용 Pitch값
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch = 0.f;
	
	// AimOffset용 Yaw와 Pitch값 업데이트 함수
	void UpdateAimOffset(float DeltaSeconds);
};
