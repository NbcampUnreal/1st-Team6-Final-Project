// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/NS_QuickSlotSlotWidget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "Inventory UI/Inventory/NS_QuickSlotKey.h"
#include "Inventory UI/Inventory/DragItemVisual.h"
#include "Inventory UI/Inventory/ItemDragDropOperation.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Item/NS_ItemDataStruct.h"


void UNS_QuickSlotSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UNS_QuickSlotSlotWidget::SetAssignedItem(const FNS_ItemDataStruct* ItemData, int32 Quantity)
{
    if (ItemIcon && ItemData)
    {
        ItemIcon->SetBrushFromTexture(ItemData->ItemAssetData.Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }

    if (AmountText)
    {
        if (Quantity > 1)
        {
            AmountText->SetText(FText::AsNumber(Quantity));
            AmountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            AmountText->SetText(FText::GetEmpty());
            AmountText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UNS_QuickSlotSlotWidget::SetSlotIndex(int32 Index)
{
    if (QuickSlotKey)
    {
        QuickSlotKey->SetSlotIndex(Index);
    }
}

void UNS_QuickSlotSlotWidget::ClearAssignedItem()
{
    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (AmountText)
    {
        AmountText->SetText(FText::GetEmpty());
        AmountText->SetVisibility(ESlateVisibility::Collapsed);
    }
}