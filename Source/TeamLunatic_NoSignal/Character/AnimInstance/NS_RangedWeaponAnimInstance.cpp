#include "NS_RangedWeaponAnimInstance.h"
#include "Item/NS_BaseRangedWeapon.h"

void UNS_RangedWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// EquipedWeaponComponent에서 무기 장착함수인 SwapWeapon()에서 원거리 무기를 장착하면 자동으로 Cast해주지만 한번더 Cast
	if (AActor* MeshOwner = GetOwningActor())
	{
		if (ANS_BaseRangedWeapon* Ranged = Cast<ANS_BaseRangedWeapon>(MeshOwner))
		{
			OwnerCharacter = Cast<ANS_PlayerCharacterBase>( Ranged->GetOwner() );
		}
	}
}

void UNS_RangedWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 만약 초기화 시 할당되지 않았을 수 있기때문에 다시 캐스트해줘야 멀티에서 적용 됨
	if (!OwnerCharacter)
	{
		if (AActor* MeshOwner = GetOwningActor())
		{
			if (ANS_BaseRangedWeapon* Ranged = Cast<ANS_BaseRangedWeapon>(MeshOwner))
			{
				OwnerCharacter = Cast<ANS_PlayerCharacterBase>( Ranged->GetOwner() );
			}
		}
	}

	// 캐릭터를 캐스트했으면 IsReload변수값을 IsReloading변수에 계속 값 저장
	if (OwnerCharacter)
	{
		bIsReloading = OwnerCharacter->IsReload;
	}
}