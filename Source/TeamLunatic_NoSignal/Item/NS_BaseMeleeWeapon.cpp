#include "Item/NS_BaseMeleeWeapon.h"
#include "Components/StaticMeshComponent.h"

ANS_BaseMeleeWeapon::ANS_BaseMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("I Can'tSee Mesh");
	ItemStaticMesh->SetOwnerNoSee(true);
	ItemStaticMesh->SetOnlyOwnerSee(false);

	ArmsMesh = CreateDefaultSubobject<UStaticMeshComponent>("I See Mesh");
	ArmsMesh->SetOwnerNoSee(false);
	ArmsMesh->SetOnlyOwnerSee(true);
}

void ANS_BaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
	}

	ArmsMesh->SetStaticMesh(ItemMesh);
	
	ItemStaticMesh->SetStaticMesh(ItemMesh);

	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}


