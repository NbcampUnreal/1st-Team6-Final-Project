#include "Character/AnimInstance/NS_RangeWeaponAnimInstance.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "GameFramework/Pawn.h"

void UNS_RangeWeaponAnimInstance::NativeInitializeAnimation()
{
	// 소유자 캐릭터 얻기
	APawn* OwnerPawn = TryGetPawnOwner();
	if (OwnerPawn)
	{
		OwnerCharacter = Cast<ANS_PlayerCharacterBase>(OwnerPawn);
	}
}

void UNS_RangeWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		APawn* OwnerPawn = TryGetPawnOwner();
		if (OwnerPawn)
		{
			OwnerCharacter = Cast<ANS_PlayerCharacterBase>(OwnerPawn);
		}
	}

	if (OwnerCharacter)
	{
		// 장착된 무기 컴포넌트에서 IsAttack 상태 확인
		if (OwnerCharacter->EquipedWeaponComp)
		{
			bIsFiring = OwnerCharacter->EquipedWeaponComp->IsAttack;
		}
	}
}
