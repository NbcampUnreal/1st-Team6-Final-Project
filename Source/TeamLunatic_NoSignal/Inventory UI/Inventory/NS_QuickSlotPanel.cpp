// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Inventory UI/Inventory/NS_QuickSlotSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"

void UNS_QuickSlotPanel::NativeConstruct()
{
    Super::NativeConstruct();

    TryBindQuickSlotPanel();
}

void UNS_QuickSlotPanel::InitializeSlots()
{
    if (!SlotBox || !QuickSlotComponent || !SlotWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuickSlotPanel] 초기화 실패 - 필수 요소 누락"));
        return;
    }

    SlotBox->ClearChildren();

    const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots = QuickSlotComponent->GetQuickSlots();
    const int32 SlotCount = QuickSlots.Num();

    for (int32 i = 0; i < SlotCount; ++i)
    {
        UNS_QuickSlotSlotWidget* NewSlot = CreateWidget<UNS_QuickSlotSlotWidget>(this, SlotWidgetClass);
        if (NewSlot)
        {
            NewSlot->SetSlotIndex(i); // 인덱스 표시용 (예: 1~5번 등)
            SlotBox->AddChildToHorizontalBox(NewSlot);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[QuickSlotPanel] 슬롯 %d개 생성 완료"), SlotCount);
}

// 퀵슬롯 컴포넌트 바인딩 시도 및 실패 시 재시도
void UNS_QuickSlotPanel::TryBindQuickSlotPanel()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn)
    {
        if (++RetryCount <= 10)
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() {
                TryBindQuickSlotPanel();
                }));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[UI-Bind] QuickSlotPanel 바인딩 실패 - 최대 재시도 초과"));
        }
        return;
    }

    QuickSlotComponent = Pawn->FindComponentByClass<UNS_QuickSlotComponent>();
    if (!QuickSlotComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[UI-Bind] QuickSlotComponent를 찾을 수 없음"));
        return;
    }

    QuickSlotComponent->QuickSlotUpdated.AddUObject(this, &UNS_QuickSlotPanel::OnQuickSlotDataUpdated);
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateLambda([this]()
            {
                if (QuickSlotComponent)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[QuickSlotPanel] 다음 틱에 슬롯 생성 재시도"));
                    InitializeSlots();
                    RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
                }
            })
    );

    UE_LOG(LogTemp, Warning, TEXT("[UI-Bind] QuickSlotComponent 바인딩 성공"));
    RetryCount = 0;
}

// 전체 슬롯 UI 갱신
void UNS_QuickSlotPanel::RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots)
{
    const int32 NumSlots = SlotBox->GetChildrenCount();

    for (int32 i = 0; i < NumSlots; ++i)
    {
        if (UWidget* Child = SlotBox->GetChildAt(i))
        {
            if (UNS_QuickSlotSlotWidget* QSlot = Cast<UNS_QuickSlotSlotWidget>(Child))
            {
                if (QuickSlots.IsValidIndex(i) && QuickSlots[i])
                {
                    QSlot->SetAssignedItem(QuickSlots[i]->GetItemData(), QuickSlots[i]->GetQuantity());
                }
                else
                {
                    QSlot->ClearAssignedItem();
                }
            }
        }
    }
}

// 델리게이트 바인딩 시 호출되는 슬롯 UI 갱신 함수
void UNS_QuickSlotPanel::OnQuickSlotDataUpdated()
{
    if (QuickSlotComponent)
    {
        RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
        UE_LOG(LogTemp, Warning, TEXT("[QuickSlotPanel] 슬롯 데이터 변경 감지 - UI 갱신"));
    }
}


