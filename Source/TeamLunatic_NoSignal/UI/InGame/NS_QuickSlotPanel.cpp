#include "UI/InGame/NS_QuickSlotPanel.h"
#include "UI/InGame/NS_QuickSlotSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/Components/NS_QuickSlotComponent.h"

void UNS_QuickSlotPanel::NativeConstruct()
{
    Super::NativeConstruct();
    TryBindQuickSlotPanel();
}

void UNS_QuickSlotPanel::InitializeSlots()
{
    if (!SlotBox || !QuickSlotComponent || !SlotWidgetClass)
    {
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
            NewSlot->SetSlotIndex(i);
            SlotBox->AddChildToHorizontalBox(NewSlot);
        }
    }
}

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
        return;
    }

    QuickSlotComponent = Pawn->FindComponentByClass<UNS_QuickSlotComponent>();
    if (!QuickSlotComponent)
    {
        return;
    }

    QuickSlotComponent->QuickSlotUpdated.AddUObject(this, &UNS_QuickSlotPanel::OnQuickSlotDataUpdated);
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateLambda([this]()
        {
            if (QuickSlotComponent)
            {
                InitializeSlots();
                RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
            }
        })
    );

    RetryCount = 0;
}

void UNS_QuickSlotPanel::RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots)
{
    if (!SlotBox)
    {
        return;
    }
    
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

void UNS_QuickSlotPanel::OnQuickSlotDataUpdated()
{
    if (QuickSlotComponent && SlotBox)
    {
        RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
    }
}