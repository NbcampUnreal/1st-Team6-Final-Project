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
	

	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}


