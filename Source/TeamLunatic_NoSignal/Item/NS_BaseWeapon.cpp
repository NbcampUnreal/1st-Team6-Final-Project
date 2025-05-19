#include "Item/NS_BaseWeapon.h"

ANS_BaseWeapon::ANS_BaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANS_BaseWeapon::Attack()
{
}

// Called when the game starts or when spawned
void ANS_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item DataTable assigned on %s"), *GetName());
		return;
	}
	
	const FNS_ItemDataStruct* WeaponData = ItemsDataTable->FindRow<FNS_ItemDataStruct>(WeaponDataRowName, TEXT(""));

	if (WeaponData)
	{
		WeaponType = WeaponData->WeaponType;
		Weapondamage = WeaponData->WeaponData.Damage;
		Durability = WeaponData->WeaponData.Durability;
		Current_Durability = Durability;
		EquipSound = WeaponData->ItemAssetData.UseSound;
		StaticMesh = WeaponData->ItemAssetData.StaticMesh;
		SkeletalMesh = WeaponData->ItemAssetData.SkeletalMesh;
	}
}

// Called every frame
void ANS_BaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}



