#include "Item/NS_BaseMeleeWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

ANS_BaseMeleeWeapon::ANS_BaseMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HideWeaponMesh"));
	RootComponent = ItemStaticMesh;
	ItemStaticMesh->SetOnlyOwnerSee(false);
	ItemStaticMesh->CastShadow = true;
	ItemStaticMesh->bCastDynamicShadow = true;

	ArmsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = ArmsMesh;
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->CastShadow = false;
	ArmsMesh->bCastDynamicShadow = false;
}

void ANS_BaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
	}

	ItemStaticMesh->SetStaticMesh(ItemMesh);

	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}


