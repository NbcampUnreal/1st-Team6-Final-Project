// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory UI/Inventory/InventoryItemSlot.h"
#include "Inventory UI/Inventory/ItemDragDropOperation.h"
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

void UInventoryPanel::NativeConstruct()
{
    Super::NativeConstruct();

    // 이 시점에서는 GetOwningPlayerPawn()이 유효할 가능성이 높음
    TryBindInventory();
}

void UInventoryPanel::TryBindInventory()
{
    PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        InventoryReference = PlayerCharacter->GetInventory();
        if (InventoryReference)
        {
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
            RefreshInventory(); // 초기 UI 갱신

            UE_LOG(LogTemp, Warning, TEXT(" TryBindInventory 바인딩 성공: %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT(" InventoryReference is nullptr"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT(" PlayerCharacter is nullptr"));
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
            UE_LOG(LogTemp, Warning, TEXT("[UI] 슬롯 추가: %s | Row: %s | 아이콘: %s"),
                *InventoryItem->GetName(),
                *InventoryItem->ItemDataRowName.ToString(),
                InventoryItem->AssetData.Icon ? TEXT("O") : TEXT("X"));
            UE_LOG(LogTemp, Warning, TEXT("InventoryContent Num = %d"), InventoryReference->GetInventoryContents().Num());
        }
    }
    SetInfoText();
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

    if (ItemDragDrop->SourceItem && InventoryReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("Detected an Item drop on InventoryPanel."))

            return true;
    }

    return false;
}
