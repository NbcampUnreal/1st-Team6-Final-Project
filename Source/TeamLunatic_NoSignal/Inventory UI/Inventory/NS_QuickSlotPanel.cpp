// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Inventory UI/Inventory/NS_QuickSlotSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"

void UNS_QuickSlotPanel::NativeConstruct()
{
    Super::NativeConstruct();

    TryBindQuickSlot();
    if (SlotWidgets.Num() == 0)
    {
        RefreshQuickSlot();
    }
}

void UNS_QuickSlotPanel::TryBindQuickSlot()
{
    ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 바인딩 실패: PlayerCharacter nullptr - 0.05초 후 재시도"));

        // 바인딩 실패 시, 0.05초 후 재시도
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, FTimerDelegate::CreateLambda([this]()
            {
                TryBindQuickSlot(); 
            }), 0.05f, false);

        return;
    }

    PlayerCharacter->QuickSlotPanel = this;

    // 슬롯 UI 생성
    RefreshQuickSlot();

    UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 바인딩 성공 - 위젯 이름: %s, 플레이어: %s"),
        *GetName(),
        *PlayerCharacter->GetName());
}

void UNS_QuickSlotPanel::RefreshQuickSlot()
{
    if (!SlotBox || !SlotWidgetClass) return;

    if (SlotWidgets.Num() == 0)
    {
        // 최초 1회만 생성
        for (int32 i = 0; i < 5; ++i)
        {
            UNS_QuickSlotSlotWidget* NewSlot = CreateWidget<UNS_QuickSlotSlotWidget>(this, SlotWidgetClass);
            if (NewSlot)
            {
                NewSlot->SetSlotIndex(i);
                SlotBox->AddChildToHorizontalBox(NewSlot);
                SlotWidgets.Add(NewSlot);
            }
        }
    }

    // 슬롯 갱신 (예: 아이콘, 수량 등)
    for (UNS_QuickSlotSlotWidget* SlotWidget : SlotWidgets)
    {
        if (SlotWidget)
        {
            SlotWidget->UpdateSlotDisplay(); // 또는 다른 처리
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] RefreshQuickSlot 완료 - Slot 수: %d"), SlotWidgets.Num());
}

bool UNS_QuickSlotPanel::IsItemAlreadyAssigned(UNS_InventoryBaseItem* Item) const
{
    if (!Item) return false;

    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        const auto* Assigned = SlotWidgets[i] ? SlotWidgets[i]->GetAssignedItem() : nullptr;

        if (Assigned && Assigned->ItemDataRowName == Item->ItemDataRowName)
        {
            UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 슬롯 %d에 이미 등록된 아이템: %s"), i, *Item->ItemDataRowName.ToString());
            return true;
        }
    }

    return false;
}

void UNS_QuickSlotPanel::AssignItemToSlot(int32 SlotIndex, UNS_InventoryBaseItem* Item)
{
    if (!SlotWidgets.IsValidIndex(SlotIndex) || !Item) return;

    // 이미 같은 아이템이 해당 슬롯에 있으면 무시
    const auto* ExistingItem = SlotWidgets[SlotIndex]->GetAssignedItem();
    if (ExistingItem && ExistingItem->ItemDataRowName == Item->ItemDataRowName)
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 슬롯 %d에 이미 동일 아이템 존재: %s"), SlotIndex, *Item->ItemDataRowName.ToString());
        return;
    }

    // 전체 슬롯 중복 체크
    if (IsItemAlreadyAssigned(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 중복 등록 방지: 아이템 %s 이미 다른 슬롯에 등록됨"), *Item->GetName());
        return;
    }

    SlotWidgets[SlotIndex]->SetAssignedItem(Item);
    UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 슬롯 %d에 아이템 %s 직접 배정"), SlotIndex, *Item->ItemDataRowName.ToString());
}

bool UNS_QuickSlotPanel::AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item)
{
    if (!Item)
    {
        return false;
    }

    if (IsItemAlreadyAssigned(Item))
    {
        return false;
    }

    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        auto* SlotWidget = SlotWidgets[i];
        if (!SlotWidget)
        {
            continue;
        }

        if (SlotWidget->IsEmpty())
        {
            SlotWidget->SetAssignedItem(Item);
            return true;
        }
        else
        {
            const auto* ExistingItem = SlotWidget->GetAssignedItem();
        }
    }
    return false;
}

UNS_InventoryBaseItem* UNS_QuickSlotPanel::GetItemInSlot(int32 SlotIndex) const
{
    if (SlotWidgets.IsValidIndex(SlotIndex))
    {
        return SlotWidgets[SlotIndex]->GetAssignedItem(); // 슬롯이 보유한 아이템 반환
    }
    return nullptr;
}

void UNS_QuickSlotPanel::RemoveItemFromSlot(UNS_InventoryBaseItem* Item)
{
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 제거할 아이템이 nullptr입니다."));
        return;
    }

    for (UNS_QuickSlotSlotWidget* QSlot : SlotWidgets)
    {
        if (!QSlot || !QSlot->GetAssignedItem()) continue;

        const UNS_InventoryBaseItem* AssignedItem = QSlot->GetAssignedItem();

        // ItemDataRowName 기준 비교 (포인터 주소 비교 대신)
        if (AssignedItem->ItemDataRowName == Item->ItemDataRowName)
        {
            QSlot->ClearAssignedItem();
            UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] %s 퀵슬롯에서 제거됨"), *Item->GetName());
            break;
        }
    }
}

