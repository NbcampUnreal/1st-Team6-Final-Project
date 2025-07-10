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
#include "Character/Components/NS_StatusComponent.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"

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

	DOREPLIFETIME(UNS_InventoryBaseItem, OwingInventory);
	DOREPLIFETIME(UNS_InventoryBaseItem, Quantity);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemDataRowName);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemType);
	DOREPLIFETIME(UNS_InventoryBaseItem, WeaponType);
	DOREPLIFETIME(UNS_InventoryBaseItem, WeaponData);
	DOREPLIFETIME(UNS_InventoryBaseItem, ItemName);
	DOREPLIFETIME(UNS_InventoryBaseItem, NumericData);
	DOREPLIFETIME(UNS_InventoryBaseItem, AssetData);
	DOREPLIFETIME(UNS_InventoryBaseItem, TextData);
	DOREPLIFETIME(UNS_InventoryBaseItem, CurrentAmmo);
}
// 현재 아이템을 복제해 반환
UNS_InventoryBaseItem* UNS_InventoryBaseItem::CreateItemCopy()
{
	// Outer가 없을 경우 로그 출력 후 종료
	UObject* OuterObject = OwingInventory ? Cast<UObject>(OwingInventory) : GetOuter();
	if (!OuterObject)
	{
		OuterObject = GetTransientPackage();
		UE_LOG(LogTemp, Warning, TEXT("[CreateItemCopy] 유효한 Outer가 없어 GetTransientPackage()를 사용합니다. RowName: %s"), *ItemDataRowName.ToString());
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
	ItemCopy->CurrentAmmo = this->CurrentAmmo;
	ItemCopy->ConsumableItemAssetData = this->ConsumableItemAssetData;
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
	}
}
// 데이터 테이블로부터 해당 아이템의 데이터를 조회
const FNS_ItemDataStruct* UNS_InventoryBaseItem::GetItemData() const
{
	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetItemData: 데이터 테이블이 없어서 GameInstance에서 가져오기 시도"));
		if (const UWorld* World = GetWorld())
		{
			if (const UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()))
			{
				ItemsDataTable = GI->GlobalItemDataTable;
				UE_LOG(LogTemp, Warning, TEXT("GetItemData: GameInstance에서 데이터 테이블 가져옴 - %s"), 
					ItemsDataTable ? TEXT("성공") : TEXT("실패"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("GetItemData: GameInstance를 찾을 수 없음"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetItemData: World를 찾을 수 없음"));
		}
	}

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("GetItemData: 데이터 테이블이 없음"));
		return nullptr;
	}

	if (ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("GetItemData: ItemDataRowName이 None임"));
		return nullptr;
	}

	const FNS_ItemDataStruct* ItemData = ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("GetItemData: 데이터 테이블에서 '%s' 항목을 찾을 수 없음"), *ItemDataRowName.ToString());
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("GetItemData: '%s' 항목 찾음, 아이콘 유무: %s"), 
		*ItemDataRowName.ToString(), 
		ItemData->ItemAssetData.Icon ? TEXT("있음") : TEXT("없음"));

	return ItemData;
}
// 아이템 사용 시 타입에 따라 분기 처리
void UNS_InventoryBaseItem::OnUseItem(ANS_PlayerCharacterBase* Character)
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
	// 아이템 타입에 따른 처리
	switch (ItemData->ItemType)
	{
	case EItemType::Misc: 
	case EItemType::Consumable:
	case EItemType::Medical:
	case EItemType::Utility: // 소모품 처리
		UseConsumableItem_Server(Character, ItemDataRowName);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("[OnUseItem] 사용 처리되지 않은 아이템 타입입니다: %d"), (uint8)ItemData->ItemType);
		break;
	}
}

void UNS_InventoryBaseItem::UseConsumableItem_Server_Implementation(ANS_PlayerCharacterBase* Character, FName InItemDatatRowName)
{
	UseConsumableItem_Multicast(Character, InItemDatatRowName);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[UseConsumableItem] 찾음.")), true, true, FLinearColor(255, 0, 0, 255), 30.f);
}

void UNS_InventoryBaseItem::UseConsumableItem_Multicast_Implementation(ANS_PlayerCharacterBase* Character, FName InItemDataRowName)
{
	if (!Character) return;

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

	const FNS_ItemDataStruct* ItemData = ItemsDataTable->FindRow<FNS_ItemDataStruct>(InItemDataRowName, TEXT(""));

	// 상태 회복 처리
	if (UNS_StatusComponent* State = Character->StatusComp)
	{
		State->AddHealthGauge(ItemData->ItemStates.HealAmount);
		State->AddStamina(ItemData->ItemStates.StaminaRecovery);

		UE_LOG(LogTemp, Log, TEXT("[UseConsumableItem] 체력 +%.1f, 스태미나 +%.1f"),
			ItemData->ItemStates.HealAmount,
			ItemData->ItemStates.StaminaRecovery
		);
	}

	if (ItemData->ConsumableItemAssetData.UseSound)
	{
		if (Character)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UseConsumableItem] '%s' 찾음."), *ItemData->ConsumableItemAssetData.UseSound->GetName());
			Character->PlaySoundOnCharacter_Multicast(ItemData->ConsumableItemAssetData.UseSound);
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[UseConsumableItem] '%s' 찾음."), *ItemData->ConsumableItemAssetData.UseSound->GetName()), true, true, FLinearColor(255, 0, 0, 255), 30.f);
		}
	}

	// 무게까지 포함한 수량 감소
	if (OwingInventory)
	{
		const int32 Removed = OwingInventory->RemoveAmountOfItem(this, 1);
		if (Removed > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UseConsumableItem] '%s' 사용됨. 제거된 수량: %d"), *ItemData->ItemTextData.ItemName.ToString(), Removed);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UseConsumableItem] OwingInventory가 null입니다. 아이템 제거 실패"));
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

int32 UNS_InventoryBaseItem::GetQuantity() const
{
	return Quantity;
}

FText UNS_InventoryBaseItem::GetItemName() const
{
	// 데이터 테이블에서 아이템 정보 가져오기
	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetItemName: 데이터 테이블에서 이름 가져옴 - %s"), *ItemData->ItemTextData.ItemName.ToString());
		return ItemData->ItemTextData.ItemName;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("GetItemName: 데이터 테이블에서 이름을 가져올 수 없음, 기본값 사용 - %s"), *ItemName.ToString());
	return ItemName;
}

float UNS_InventoryBaseItem::GetWeight() const
{
	// 데이터 테이블에서 무게 정보 가져오기
	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (ItemData)
	{
		return ItemData->ItemNumericData.Weight;
	}
	return Weight;
}

UTexture2D* UNS_InventoryBaseItem::GetItemIcon() const
{
	// 데이터 테이블에서 아이콘 정보 가져오기
	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (ItemData && ItemData->ItemAssetData.Icon)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetItemIcon: 데이터 테이블에서 아이콘 가져옴 - %s"), 
			ItemData->ItemAssetData.Icon ? *ItemData->ItemAssetData.Icon->GetName() : TEXT("NULL"));
		return ItemData->ItemAssetData.Icon;
	}
	
	// 데이터 테이블에 아이콘이 없으면 인스턴스 변수 사용
	if (Icon)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetItemIcon: 인스턴스 아이콘 사용 - %s"), *Icon->GetName());
		return Icon;
	}
	
	// 아이콘이 없는 경우
	UE_LOG(LogTemp, Error, TEXT("GetItemIcon: 아이콘을 찾을 수 없음 - ItemDataRowName: %s"), *ItemDataRowName.ToString());
	return nullptr;
}