#include "Item/NS_BaseWeapon.h"
#include "GameFramework/Actor.h"

ANS_BaseWeapon::ANS_BaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void ANS_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		Damage = ItemData->WeaponData.Damage;
		WeaponType = ItemData->WeaponType;
		Durability = ItemData->WeaponData.Durability;
		Current_Durability = Durability;
	}
}


