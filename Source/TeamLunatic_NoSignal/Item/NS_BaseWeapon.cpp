#include "Item/NS_BaseWeapon.h"

ANS_BaseWeapon::ANS_BaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANS_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		Durability = ItemData->WeaponData.Durability;
		Current_Durability = Durability;
	}
}



