// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/NS_InventoryBaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "Item/NS_BaseWeapon.h"
#include "Engine/DataTable.h"
#include "GameFlow/NS_GameInstance.h"

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
	DOREPLIFETIME(UNS_InventoryBaseItem, WeaponData);
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
	if (!ItemsDataTable)
	{
		if (const UWorld* World = GetWorld())
		{
			if (const UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()))
			{
				ItemsDataTable = GI->GlobalItemDataTable;
				UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] GameInstance에서 DataTable 재설정: %s"),
					*GetNameSafe(ItemsDataTable));
			}
		}
	}
	// 먼저 RowName과 DataTable 상태 확인
	UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] Called on: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] ItemDataRowName: %s"), *ItemDataRowName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] ItemsDataTable: %s"),
		ItemsDataTable ? *ItemsDataTable->GetName() : TEXT("NULL"));

	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[OnUseItem] 데이터 테이블 또는 RowName 없음"));
		return;
	}

	// 아이템 데이터가 정상적으로 파싱되었는지 확인
	UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] ItemName: %s, Type: %d, WeaponClass: %s"),
		*ItemData->ItemTextData.ItemName.ToString(),
		(int32)ItemData->ItemType,
		*GetNameSafe(ItemData->WeaponActorClass));

	// 실제 장비 장착
	if (ItemData->ItemType == EItemType::Equipment && ItemData->WeaponActorClass)
	{
		if (auto* Player = Cast<ANS_PlayerCharacterBase>(OwingInventory ? OwingInventory->GetOwner() : nullptr))
		{
			if (auto* WeaponComp = Player->FindComponentByClass<UNS_EquipedWeaponComponent>())
			{
				UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] 장착 시도: %s"), *ItemData->WeaponActorClass->GetName());

				WeaponComp->SwapWeapon(ItemData->WeaponActorClass);
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[OnUseItem] WeaponComponent 없음"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[OnUseItem] Player 캐스팅 실패"));
		}
	}
}

bool UNS_InventoryBaseItem::IsSupportedForNetworking() const
{
	return true;
}
