#include "NS_RangedWeaponAnimInstance.h"
#include "Item/NS_BaseRangedWeapon.h"

void UNS_RangedWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (AActor* MeshOwner = GetOwningActor())
	{
		// BaseRangedWeapon을 캐스트하고
		if (ANS_BaseRangedWeapon* Ranged = Cast<ANS_BaseRangedWeapon>(MeshOwner))
		{
			// 캐릭터를 캐스트해서 변수로 저장
			OwnerCharacter = Cast<ANS_PlayerCharacterBase>( Ranged->GetOwner() );
		}
	}
}

void UNS_RangedWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Tick으로 캐릭터에 IsReload변수값을 가져와서 IsReloading변수가 값 저장
	if (OwnerCharacter)
	{
		bIsReloading = OwnerCharacter->IsReload;
	}
}


