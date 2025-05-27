// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryTooltip.h"
#include "Item/NS_BaseItem.h"
#include "Inventory UI/Inventory/InventoryItemSlot.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	const ANS_BaseItem* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	switch (ItemBeingHovered->ItemType)
	{
		// 소모품
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString(TEXT("소모품")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

		// 엔딩 조건
	case EItemType::EndingTrigger:
		ItemType->SetText(FText::FromString(TEXT("엔딩 조건")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

		// 장비
	case EItemType::Equipment:
		ItemType->SetText(FText::FromString(TEXT("장비")));
		MaxStack->SetVisibility(ESlateVisibility::Collapsed);
		UsageText->SetVisibility(ESlateVisibility::Collapsed);
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Visible);

		switch (ItemBeingHovered->WeaponType)
		{
		case EWeaponType::Melee:
			WeaponType->SetText(FText::FromString(TEXT("근접 무기")));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EWeaponType::Ranged:
			WeaponType->SetText(FText::FromString(TEXT("원거리 무기")));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EWeaponType::Ammo:
			WeaponType->SetText(FText::FromString(TEXT("탄약")));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			WeaponType->SetText(FText::FromString(TEXT("기타 무기")));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		}
		break;

		// 제작 재료
	case EItemType::Material:
		ItemType->SetText(FText::FromString(TEXT("제작 재료")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

		// 치료품
	case EItemType::Medical:
		ItemType->SetText(FText::FromString(TEXT("치료 아이템")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

		// 기타
	case EItemType::Misc:
		ItemType->SetText(FText::FromString(TEXT("기타")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

		// 도구
	case EItemType::Utility:
		ItemType->SetText(FText::FromString(TEXT("도구")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;

	default:
		ItemType->SetText(FText::FromString(TEXT("알 수 없음")));
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}


	ItemName->SetText(ItemBeingHovered->TextData.ItemName);
	UsageText->SetText(ItemBeingHovered->TextData.InteractionText);
	ItemDescription->SetText(ItemBeingHovered->TextData.ItemDescription);
	StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->WeaponData.Damage));

	const FString WeightInfo = {FString(TEXT("무게: ")) + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };

	StackWeight->SetText(FText::FromString(WeightInfo));

	if (ItemBeingHovered->NumericData.isStackable)
	{
		const FString StackInfo = {FString(TEXT("용량: ")) + FString::FromInt(ItemBeingHovered->NumericData.MaxStack) };

		MaxStack->SetText(FText::FromString(StackInfo));
		MaxStackHorizontal->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		MaxStackHorizontal->SetVisibility(ESlateVisibility::Collapsed);
	}
}
