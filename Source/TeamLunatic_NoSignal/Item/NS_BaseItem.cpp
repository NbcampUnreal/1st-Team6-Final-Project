#include "Item/NS_BaseItem.h"
#include "Inventory/InventoryComponent.h"

ANS_BaseItem::ANS_BaseItem() : bisCopy(false), bisPickup(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANS_BaseItem::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item DataTable assigned on %s"), *GetName());
		return;
	}

	if (ItemData)
	{
		ItemType = ItemData->ItemType;
		WeaponType = ItemData->WeaponType;
		WeaponData = ItemData->WeaponData;
		ItemName = ItemData->ItemTextData.ItemName;
		NumericData = ItemData->ItemNumericData;
		GetItemSound = ItemData->ItemAssetData.GetSound;
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		Icon = ItemData->ItemAssetData.Icon;
	}

	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Name = ItemName;
	InstanceInteractableData.Action = FText::FromString("to pick up");
}

void ANS_BaseItem::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
}

ANS_BaseItem* ANS_BaseItem::CreateItemCopy()
{
	ANS_BaseItem* ItemCopy = NewObject<ANS_BaseItem>(this, GetClass());

	ItemCopy->ItemDataRowName = this->ItemDataRowName;
	ItemCopy->ItemName = this->ItemName;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->WeaponType = this->WeaponType;
	ItemCopy->WeaponData = this->WeaponData;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->bisCopy = true;

	return ItemCopy;
}

void ANS_BaseItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, NumericData.isStackable ? NumericData.MaxStack : 1);

		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				//OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}

const FNS_ItemDataStruct* ANS_BaseItem::GetItemData() const
{
	if (!ItemsDataTable || ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블 또는 RowName 없음"));
		return nullptr;
	}

	return ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));
}

// Called every frame
void ANS_BaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANS_BaseItem::OnUseItem()
{

}

void ANS_BaseItem::BeginFocus()
{
	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetRenderCustomDepth(true);
	}
}

void ANS_BaseItem::EndFocus()
{
	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetRenderCustomDepth(false);
	}
}

