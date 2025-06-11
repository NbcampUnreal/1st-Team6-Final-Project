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


void UNS_QuickSlotSlotWidget::SetAssignedItem(UNS_InventoryBaseItem* Item)
{
    AssignedItem = Item;

    UE_LOG(LogTemp, Warning, TEXT("[QuickSlotSlot] SetAssignedItem 호출됨 - 아이템: %s (%s)"),
        Item ? *Item->GetName() : TEXT("nullptr"),
        Item ? *Item->GetClass()->GetName() : TEXT("null"));
    UpdateSlotDisplay();
}

void UNS_QuickSlotSlotWidget::UseAssignedItem()
{
    if (AssignedItem)
    {
        if (auto* Player = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn()))
        {
            Player->Server_UseQuickSlotItem(AssignedItem->ItemDataRowName);
        }
    }
}

void UNS_QuickSlotSlotWidget::SetSlotIndex(int32 InIndex)
{
    SlotIndex = InIndex;
    if (QuickSlotKey)
    {
        QuickSlotKey->SetSlotIndex(InIndex);
    }
}

void UNS_QuickSlotSlotWidget::ClearAssignedItem()
{
    AssignedItem = nullptr;
    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(nullptr);
    }
    if (AmountText)
    {
        AmountText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UNS_QuickSlotSlotWidget::UpdateSlotDisplay()
{
    if (!AssignedItem)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuickSlotSlot] AssignedItem == nullptr"));
    }

    if (!ItemIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuickSlotSlot] ItemIcon == nullptr (UMG 바인딩 확인 필요)"));
    }

    if (ItemIcon && AssignedItem)
    {
        if (AssignedItem->AssetData.Icon)
        {
            ItemIcon->SetVisibility(ESlateVisibility::Visible);
            ItemIcon->SetBrushFromTexture(AssignedItem->AssetData.Icon);
            UE_LOG(LogTemp, Warning, TEXT("[QuickSlotSlot] 아이콘 적용 완료 - 아이템: %s, 아이콘: %s"),
                *AssignedItem->GetName(),
                *AssignedItem->AssetData.Icon->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[QuickSlotSlot] 아이템 %s 의 AssetData.Icon == nullptr"), *AssignedItem->GetName());
        }
    }

    if (!AmountText)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuickSlotSlot] AmountText == nullptr (UMG 바인딩 확인 필요)"));
    }

    if (AmountText)
    {
        if (AssignedItem && AssignedItem->NumericData.isStackable)
        {
            AmountText->SetText(FText::AsNumber(AssignedItem->Quantity));
            AmountText->SetVisibility(ESlateVisibility::Visible);

            UE_LOG(LogTemp, Warning, TEXT("[QuickSlotSlot] 수량 표시 - 아이템: %s, 수량: %d"),
                *AssignedItem->GetName(), AssignedItem->Quantity);
        }
        else
        {
            AmountText->SetVisibility(ESlateVisibility::Collapsed);
            UE_LOG(LogTemp, Warning, TEXT("[QuickSlotSlot] 수량 비표시 - 아이템: %s (Stackable: false)"),
                AssignedItem ? *AssignedItem->GetName() : TEXT("null"));
        }
    }
}


bool UNS_QuickSlotSlotWidget::IsEmpty() const
{
    return AssignedItem == nullptr;
}

FReply UNS_QuickSlotSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && AssignedItem)
    {
        return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }

    return Reply;
}

void UNS_QuickSlotSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    if (DragItemVisualClass) return;

    const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
    DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
    DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

    UItemDragDropOperation* DragOp = NewObject<UItemDragDropOperation>();
    DragOp->SourceItem = ItemReference;
    DragOp->SourceInventory = ItemReference->OwingInventory;
    DragOp->DefaultDragVisual = DragVisual;
    DragOp->Pivot = EDragPivot::TopLeft;

    OutOperation = DragOp;
}

bool UNS_QuickSlotSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (UItemDragDropOperation* DropOp = Cast<UItemDragDropOperation>(InOperation))
    {
        // 자기 자신한테 드롭한 거면 무시
        if (DropOp->SourceItem == AssignedItem)
        {
            UE_LOG(LogTemp, Warning, TEXT("같은 슬롯에 드롭됨 -> 버리기 아님"));
            return false;
        }

        // 자기 자신이 아닌데 드롭됐다면 → 버리는 로직 실행
        if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn()))
        {
            if (UNS_EquipedWeaponComponent* WeaponComp = Player->FindComponentByClass<UNS_EquipedWeaponComponent>())
            {
                WeaponComp->Server_UnequipWeapon();
            }

            Player->QuickSlotPanel->RemoveItemFromSlot(DropOp->SourceItem);
            Player->DropItem_Server(DropOp->SourceItem, DropOp->SourceItem->GetQuantity());

            return true;
        }
    }
    return false;
}