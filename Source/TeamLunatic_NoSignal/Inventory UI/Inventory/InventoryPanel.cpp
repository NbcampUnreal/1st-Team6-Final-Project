// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory UI/Inventory/InventoryItemSlot.h"
#include "Inventory UI/Inventory/ItemDragDropOperation.h"
#include "Item/NS_InventoryBaseItem.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
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
            RefreshInventory();
            UE_LOG(LogTemp, Warning, TEXT("[UI-Bind] 성공: %s, 컨트롤러: %s"),
                *GetName(), *GetOwningPlayer()->GetName());
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[UI-Bind] 실패: InventoryReference가 null입니다. 위젯: %s"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[UI-Bind] 실패: PlayerCharacter nullptr"));
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
        InventoryReference->CleanUpZeroQuantityItems();

        InventoryPanel->ClearChildren();

        const auto& Contents = InventoryReference->GetInventoryContents();

        UE_LOG(LogTemp, Warning, TEXT("[UI] RefreshInventory 호출됨 - Contents.Num() = %d"), Contents.Num());

        for (UNS_InventoryBaseItem* const& InventoryItem : InventoryReference->GetInventoryContents())
        {
            if (!IsValid(InventoryItem) || InventoryItem->GetQuantity() <= 0)
                continue;

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
