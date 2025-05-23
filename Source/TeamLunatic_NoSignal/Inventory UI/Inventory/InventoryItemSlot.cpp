// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryItemSlot.h"
#include "Inventory UI/Inventory/InventoryTooltip.h"
#include "Item/NS_BaseItem.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (TooltipClass)
	{
		UInventoryTooltip* Tooltip = CreateWidget<UInventoryTooltip>(this, TooltipClass);
		Tooltip->InventorySlotBeingHovered = this;
		SetToolTip(Tooltip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		switch (ItemReference->ItemType)
		{
			//소모품
		case EItemType::Consumable:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//엔딩조건
		case EItemType::EndingTrigger:
			ItemBorder->SetBrushColor(FLinearColor::Red);
			break;
			//장비
		case EItemType::Equipment:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//제작재료
		case EItemType::Material:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//치료품
		case EItemType::Medical:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
			//기타
		case EItemType::Misc:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//도구
		case EItemType::Utility:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		default:;
		}
		ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);

		if (ItemReference->NumericData.isStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
