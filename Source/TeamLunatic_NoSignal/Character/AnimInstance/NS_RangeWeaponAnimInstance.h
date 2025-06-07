#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NS_RangeWeaponAnimInstance.generated.h"

class ANS_PlayerCharacterBase;


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_RangeWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
    
public:
	// 캐릭터 사격 중인지 여부
	UPROPERTY(BlueprintReadOnly, Category="WeaponAnim")
	bool bIsFiring = false;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	// 캐릭터 참조 저장용
	UPROPERTY()
	ANS_PlayerCharacterBase* OwnerCharacter;
};
