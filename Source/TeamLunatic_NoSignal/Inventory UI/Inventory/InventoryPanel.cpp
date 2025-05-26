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
    const FString WeightInfoValue{
        FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + "/"
        + FString::SanitizeFloat(InventoryReference->GetWeightCapacity())
    };

    const FString CapacityInfoValue{
        FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
        + FString::FromInt(InventoryReference->GetSlotsCapacity())
    };
 
    WeightInfo->SetText(FText::FromString(WeightInfoValue));
    CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
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
