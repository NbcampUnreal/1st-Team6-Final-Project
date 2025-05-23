#include "Item/NS_BaseWeapon.h"

ANS_BaseWeapon::ANS_BaseWeapon() : bisCopy(false), bisPickup(false)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANS_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		WeaponType = ItemData->WeaponType;
		WeaponDamage = ItemData->WeaponData.Damage;
		Durability = ItemData->WeaponData.Durability;
		Current_Durability = Durability;
	}
}


<<<<<<< HEAD
=======
		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}
>>>>>>> parent of 189edb63a ([Fix]인벤토리 컴포넌트, 픽업 수정 #7)

