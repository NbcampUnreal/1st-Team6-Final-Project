// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/NS_InventoryBaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "Item/NS_BaseWeapon.h"
#include "Engine/DataTable.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Item/NS_BaseMagazine.h"
#include "Item/NS_BaseAmmo.h"
#include "GameFlow/NS_GameInstance.h"

UNS_InventoryBaseItem::UNS_InventoryBaseItem() : bisCopy(false), bisPickup(false)
{
}
// 복제 여부 및 월드에서 습득 여부 초기화
void UNS_InventoryBaseItem::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
}
// 네트워크 복제 속성 정의 
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
// 현재 아이템을 복제해 반환
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

	ItemCopy->SetOwningActor(this->GetOwningActor());

	UE_LOG(LogTemp, Warning, TEXT("[CreateItemCopy] 복제된 아이템: %s, RowName: %s"),
		*ItemCopy->GetName(),
		*ItemCopy->ItemDataRowName.ToString());

	return ItemCopy;
}
// 수량 변경 시 처리
void UNS_InventoryBaseItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		// 스택 가능 여부에 따라 최대 수량 제한
		Quantity = FMath::Clamp(NewQuantity, 0, NumericData.isStackable ? NumericData.MaxStack : 1);
		// 수량이 0 이하이면 인벤토리에서 제거
		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}
// 데이터 테이블로부터 해당 아이템의 데이터를 조회
const FNS_ItemDataStruct* UNS_InventoryBaseItem::GetItemData() const
{
	if (!ItemsDataTable || ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블 또는 RowName 없음"));
		return nullptr;
	}

	return ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));
}
// 아이템 사용 시 타입에 따라 분기 처리
void UNS_InventoryBaseItem::OnUseItem()
{
	// 데이터 테이블이 없으면 GameInstance에서 재설정
	if (!ItemsDataTable)
	{
		if (const UWorld* World = GetWorld())
		{
			if (const UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()))
			{
				ItemsDataTable = GI->GlobalItemDataTable;
			}
		}
	}

	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (!ItemData) return;
	// 장비 아이템일 경우 무기/탄창/탄약 분기 처리
	if (ItemData->ItemType == EItemType::Equipment)
	{
		switch (ItemData->WeaponType)
		{
		case EWeaponType::Melee:
		case EWeaponType::Ranged:
		case EWeaponType::Pistol:
			EquipWeapon(ItemData);
			break;

		case EWeaponType::Ammo:
			UseAmmo(ItemData);
			break;

		default:
			break;
		}
	}
}
// 무기 장착 처리
void UNS_InventoryBaseItem::EquipWeapon(const FNS_ItemDataStruct* ItemData)
{
	if (!ItemData || !ItemData->WeaponActorClass) return;

	if (auto* Player = Cast<ANS_PlayerCharacterBase>(OwingInventory->GetOwner()))
	{
		if (auto* WeaponComp = Player->FindComponentByClass<UNS_EquipedWeaponComponent>())
		{
			UE_LOG(LogTemp, Log, TEXT("[EquipWeapon] 무기 장착: %s"), *ItemData->WeaponActorClass->GetName());
			WeaponComp->SwapWeapon(ItemData->WeaponActorClass);
		}
	}
}

// 탄약 사용 처리
void UNS_InventoryBaseItem::UseAmmo(const FNS_ItemDataStruct* ItemData)
{
	// 아이템 데이터나 인벤토리가 유효하지 않으면 종료
	if (!ItemData || !OwingInventory) return;

	// 인벤토리를 소유한 액터가 플레이어 캐릭터인지 확인
	if (auto* Player = Cast<ANS_PlayerCharacterBase>(OwingInventory->GetOwner()))
	{
		// 플레이어가 무기 장착 컴포넌트를 보유하고 있는지 확인
		if (auto* WeaponComp = Player->FindComponentByClass<UNS_EquipedWeaponComponent>())
		{
			// 현재 장착 중인 무기가 원거리 무기인지 확인
			if (auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(WeaponComp->CurrentWeapon))
			{
				int32 AmmoToReload = FMath::Min(Quantity, RangedWeapon->GetMaxAmmo() - RangedWeapon->GetCurrentAmmo());

				if (AmmoToReload > 0)
				{
					RangedWeapon->Reload(AmmoToReload);
					Quantity -= AmmoToReload;

					UE_LOG(LogTemp, Log, TEXT("[UseAmmo] %d발 장전됨. 남은 수량: %d"), AmmoToReload, Quantity);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[UseAmmo] 탄약이 이미 가득 참"));
				}
			}
		}
	}
}

// 네트워크에서 UObject 복제를 허용
bool UNS_InventoryBaseItem::IsSupportedForNetworking() const
{
	return true;
}

void UNS_InventoryBaseItem::SetOwningActor(AActor* NewOwner)
{
	OwningActor = NewOwner;
}

AActor* UNS_InventoryBaseItem::GetOwningActor() const
{
	return OwningActor;
}
