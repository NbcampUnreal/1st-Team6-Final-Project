#include "Item/NS_BaseThrowWeapon.h"
#include "Components/StaticMeshComponent.h"

ANS_BaseThrowWeapon::ANS_BaseThrowWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("I Can'tSee Mesh");
	ItemStaticMesh->SetOwnerNoSee(true);
	ItemStaticMesh->SetOnlyOwnerSee(false);

	ArmsMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("I See Mesh");
	ArmsMeshComp->SetOwnerNoSee(false);
	ArmsMeshComp->SetOnlyOwnerSee(true);
}

void ANS_BaseThrowWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();
}
