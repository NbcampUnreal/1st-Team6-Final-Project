// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory UI/Inventory/InventoryItemSlot.h"
#include "Item/NS_BaseItem.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        InventoryReference = PlayerCharacter->GetInventory();
        if (InventoryReference)
        {
            
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);

           
            SetInfoText();
        }
    }
}

void UInventoryPanel::SetInfoText() const
{
    WeightInfo->SetText(FText::Format(FText::FromString("{0}/{1}"),
        InventoryReference->GetInventoryTotalWeight(),
        InventoryReference->GetWeightCapacity()));
    CapacityInfo->SetText(FText::Format(FText::FromString("{0}/{1}"),
        InventoryReference->GetInventoryContents().Num(),
        InventoryReference->GetSlotsCapacity()));
}

void UInventoryPanel::RefreshInventory()
{
    if (InventoryReference && InventorySlotClass)
    {
        InventoryPanel->ClearChildren();

        for (ANS_BaseItem* const& InventoryItem : InventoryReference->GetInventoryContents())
        {
            UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
            ItemSlot->SetItemReference(InventoryItem);

            InventoryPanel->AddChildToWrapBox(ItemSlot);
        }
    }
    SetInfoText();
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}
