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
		//소모품
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString("Consumable"));
		WeaponType->SetVisibility(ESlateVisibility::Collapsed);
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		break;
		//엔딩조건
	case EItemType::EndingTrigger:
		break;
		//장비
	case EItemType::Equipment:
		ItemType->SetText(FText::FromString("Equipment"));
		WeaponType->SetVisibility(ESlateVisibility::Visible);

		switch (ItemBeingHovered->WeaponType)
		{
		case EWeaponType::Melee:
			WeaponType->SetText(FText::FromString("���� ����"));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EWeaponType::Ranged:
			WeaponType->SetText(FText::FromString("���Ÿ� ����"));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		case EWeaponType::Ammo:
			WeaponType->SetText(FText::FromString("ź��"));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			WeaponType->SetText(FText::FromString("��Ÿ"));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			break;
		}
		break;
		//제작재료
	case EItemType::Material:
		break;
		//치료품
	case EItemType::Medical:
		break;
		//기타
	case EItemType::Misc:
		break;
		//도구
	case EItemType::Utility:
		break;
	default:;
	}

	ItemName->SetText(ItemBeingHovered->TextData.ItemName);
	UsageText->SetText(ItemBeingHovered->TextData.InteractionText);
	ItemDescription->SetText(ItemBeingHovered->TextData.ItemDescription);
	StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));
	//DamageValue->SetText(FText::AsNumber(ItemBeingHovered->WeaponData.Damage));

	if (ItemBeingHovered->NumericData.isStackable)
	{
		MaxStack->SetText(FText::AsNumber(ItemBeingHovered->NumericData.MaxStack));
	}
	else
	{
		MaxStack->SetVisibility(ESlateVisibility::Collapsed);
	}
}
