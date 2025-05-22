#include "Item/NS_BaseWeapon.h"
#include "Inventory/InventoryComponent.h"

ANS_BaseWeapon::ANS_BaseWeapon() : bisCopy(false), bisPickup(false)
{
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

void ANS_BaseWeapon::SetQuantity(int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, ItemNumericData.isStackable ? ItemNumericData.MaxStack : 1);

		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}

void ANS_BaseWeapon::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
}

ANS_BaseWeapon* ANS_BaseWeapon::CreateItemCopy() const
{
	ANS_BaseWeapon* ItemCopy = NewObject<ANS_BaseWeapon>(StaticClass());

	ItemCopy->WeaponDataRowName = this->WeaponDataRowName;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemNumericData = this->ItemNumericData;
	ItemCopy->ItemAssetData = this->ItemAssetData;
	ItemCopy->ItemTextData = this->ItemTextData;
	ItemCopy->bisCopy = true;
	ItemCopy->StaticMesh = this->ItemAssetData.StaticMesh;

	return ItemCopy;
}



