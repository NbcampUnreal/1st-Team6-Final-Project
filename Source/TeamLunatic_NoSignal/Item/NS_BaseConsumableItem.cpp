#include "Item/NS_BaseConsumableItem.h"

UNS_BaseConsumableItem::UNS_BaseConsumableItem()
{
	if (ItemsDataTable && !ItemDataRowName.IsNone())
	{
		InitializeFromDataTable();
	}
}

void UNS_BaseConsumableItem::OnUseItem()
{
}

void UNS_BaseConsumableItem::ApplyConsumableEffect()
{
}

void UNS_BaseConsumableItem::InitializeFromDataTable()
{
	if (!ItemsDataTable) return;

	const FNS_ItemDataStruct* ItemData = ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));

	if (ItemData)
	{
		ItemName = ItemData->ItemTextData.ItemName;

	}
}


