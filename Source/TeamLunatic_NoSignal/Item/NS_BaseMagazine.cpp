#include "Item/NS_BaseMagazine.h"
#include "Components/StaticMeshComponent.h"

ANS_BaseMagazine::ANS_BaseMagazine()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	RootComponent = ItemStaticMesh;
}

void ANS_BaseMagazine::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		MaxAmmo = ItemData->WeaponData.MaxAmmo;
	}

	//Setting
	ItemStaticMesh->SetStaticMesh(ItemMesh);
	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;
}

void ANS_BaseMagazine::AddCurrentAmmo(int32 AddAmount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AddAmount, 0, MaxAmmo);
}

void ANS_BaseMagazine::MinusCurrentAmmo(int32 SubAmount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - SubAmount, 0, MaxAmmo);
}






