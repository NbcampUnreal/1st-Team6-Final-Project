#include "Item/NS_BaseAmmo.h"

ANS_BaseAmmo::ANS_BaseAmmo()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	RootComponent = ItemStaticMesh;
}

void ANS_BaseAmmo::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		MaxAmmo = ItemData->ItemNumericData.MaxStack;
		InstanceInteractableData.Quantity = ItemData->ItemNumericData.MaxStack;
	}

	ItemStaticMesh->SetStaticMesh(ItemMesh);
	InteractableData = InstanceInteractableData;
}

void ANS_BaseAmmo::AddCurrentAmmo(int32 AddAmount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AddAmount, 0, MaxAmmo);
}

void ANS_BaseAmmo::RemoveCurrentAmmo(int32 RemoveAmount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - RemoveAmount, 0, MaxAmmo);
}






