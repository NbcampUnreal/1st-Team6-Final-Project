#include "Item/NS_BaseRangedWeapon.h"
#include "Components/SkeletalMeshComponent.h"

ANS_BaseRangedWeapon::ANS_BaseRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemSkeletalcomponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = ItemSkeletalcomponent;
}

void ANS_BaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemSkeletalMesh = ItemData->ItemAssetData.SkeletalMesh;
	}

	ItemSkeletalcomponent->SetSkeletalMesh(ItemSkeletalMesh);

	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}

void ANS_BaseRangedWeapon::BeginFocus()
{
	if (ItemSkeletalcomponent)
	{
		ItemSkeletalcomponent->SetRenderCustomDepth(true);
		ItemSkeletalcomponent->SetCustomDepthStencilValue(1);
	}
}

void ANS_BaseRangedWeapon::EndFocus()
{
	if (ItemSkeletalcomponent)
	{
		ItemSkeletalcomponent->SetRenderCustomDepth(false);
		ItemSkeletalcomponent->SetCustomDepthStencilValue(0);
	}
}

