#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "NS_RangedWeaponAnimInstance.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_RangedWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 초기화 이벤트에서 캐스트해줄거임
	UPROPERTY(BlueprintReadOnly, Category="WeaponAnim")
	ANS_PlayerCharacterBase* OwnerCharacter = nullptr;

	// 캐릭터에 IsReload변수를 저장할 변수
	UPROPERTY(BlueprintReadOnly, Category="WeaponAnim")
	bool bIsReloading = false;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};


