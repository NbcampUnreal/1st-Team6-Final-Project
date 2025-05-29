// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/NS_InventoryBaseItem.h"
#include "Net/UnrealNetwork.h"

UNS_InventoryBaseItem::UNS_InventoryBaseItem() : bisCopy(false), bisPickup(false)
{
}

void UNS_InventoryBaseItem::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
}

void UNS_InventoryBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNS_InventoryBaseItem, Quantity);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemDataRowName);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemType);
	DOREPLIFETIME(UNS_InventoryBaseItem, WeaponType);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemName);
	DOREPLIFETIME(UNS_InventoryBaseItem, NumericData);
	DOREPLIFETIME(UNS_InventoryBaseItem, AssetData);
	DOREPLIFETIME(UNS_InventoryBaseItem, TextData);
}

UNS_InventoryBaseItem* UNS_InventoryBaseItem::CreateItemCopy()
{
	// Outer가 없을 경우 로그 출력 후 종료
	UObject* OuterObject = OwingInventory ? Cast<UObject>(OwingInventory) : GetOuter();
	if (!OuterObject)
	{
		UE_LOG(LogTemp, Error, TEXT("[CreateItemCopy] 유효한 Outer 없음. 아이템 복제 실패."));
		return nullptr;
	}

	// NewObject로 복제 생성
	UNS_InventoryBaseItem* ItemCopy = NewObject<UNS_InventoryBaseItem>(OuterObject, GetClass());
	if (!ItemCopy)
	{
		UE_LOG(LogTemp, Error, TEXT("[CreateItemCopy] NewObject 실패."));
		return nullptr;
	}

	ItemCopy->OwingInventory = this->OwingInventory;
	ItemCopy->ItemDataRowName = this->ItemDataRowName;
	ItemCopy->ItemName = this->ItemName;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->WeaponType = this->WeaponType;
	ItemCopy->WeaponData = this->WeaponData;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->ItemsDataTable = this->ItemsDataTable;
	ItemCopy->bisCopy = true;

	UE_LOG(LogTemp, Warning, TEXT("[CreateItemCopy] 복제된 아이템: %s, RowName: %s"),
		*ItemCopy->GetName(),
		*ItemCopy->ItemDataRowName.ToString());

	return ItemCopy;
}

void UNS_InventoryBaseItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, NumericData.isStackable ? NumericData.MaxStack : 1);

		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}

const FNS_ItemDataStruct* UNS_InventoryBaseItem::GetItemData() const
{
	if (!ItemsDataTable || ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블 또는 RowName 없음"));
		return nullptr;
	}

	return ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));
}

void UNS_InventoryBaseItem::OnUseItem()
{
}

bool UNS_InventoryBaseItem::IsSupportedForNetworking() const
{
	return true;
}
