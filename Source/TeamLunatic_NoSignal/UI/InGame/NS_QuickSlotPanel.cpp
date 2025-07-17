#include "UI/InGame/NS_QuickSlotPanel.h"
#include "UI/InGame/NS_QuickSlotBox.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/Components/NS_QuickSlotComponent.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_QuickSlotPanel::NativeConstruct()
{
    Super::NativeConstruct();
    // 위젯 생성 시 퀵 슬롯 패널 바인딩을 시도합니다.
    TryBindQuickSlotPanel();
}

/**
 * @brief 퀵 슬롯 패널을 퀵 슬롯 컴포넌트에 바인딩을 시도합니다.
 *        주로 플레이어 캐릭터의 퀵 슬롯 컴포넌트를 찾아 연결합니다.
 */
void UNS_QuickSlotPanel::TryBindQuickSlotPanel()
{
    // 플레이어 컨트롤러를 가져옵니다.
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    // 플레이어 폰을 가져옵니다.
    APawn* Pawn = PC->GetPawn();
    // 폰이 유효하지 않으면 재시도 로직을 수행합니다.
    if (!Pawn)
    {
        // 재시도 횟수가 10회 이하이면 다음 틱에 다시 바인딩을 시도합니다.
        if (++RetryCount <= 10)
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]() {
                TryBindQuickSlotPanel();
            }));
        }
        return;
    }

    // 폰에서 퀵 슬롯 컴포넌트를 찾아 할당합니다.
    QuickSlotComponent = Pawn->FindComponentByClass<UNS_QuickSlotComponent>();
    // 퀵 슬롯 컴포넌트가 유효하지 않으면 함수를 종료합니다.
    if (!QuickSlotComponent)
    {
        return;
    }

    // 퀵 슬롯 데이터 업데이트 델리게이트에 OnQuickSlotDataUpdated 함수를 바인딩합니다.
    QuickSlotComponent->QuickSlotUpdated.AddUObject(this, &UNS_QuickSlotPanel::OnQuickSlotDataUpdated);
    
    // 다음 틱에 슬롯 인덱스를 설정하고 퀵 슬롯을 갱신합니다.
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateLambda([this]()
        {
            if (QuickSlotComponent)
            {
                RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
            }
        })
    );

    // 바인딩 성공 시 재시도 횟수를 초기화합니다.
    RetryCount = 0;
}

/**
 * @brief 퀵 슬롯 컴포넌트에서 호출하여 UI를 갱신합니다.
 * @param QuickSlots 갱신할 퀵 슬롯 아이템들의 배열입니다.
 */
void UNS_QuickSlotPanel::RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots)
{
    // 5개의 슬롯을 각각 업데이트
    TArray<UNS_QuickSlotBox*> Slots = {Slot0, Slot1, Slot2, Slot3, Slot4};
    
    for (int32 i = 0; i < 5; ++i)
    {
        if (Slots[i])
        {
            // 슬롯 인덱스 설정 (중요!) 
            Slots[i]->SlotIndex = i;
            
            // 해당 인덱스의 아이템만 업데이트
            if (QuickSlots.IsValidIndex(i) && QuickSlots[i])
            {
                Slots[i]->SetAssignedItem(QuickSlots[i]->GetItemData(), QuickSlots[i]->GetQuantity());
            }
            else
            {
                Slots[i]->ClearAssignedItem();
            }
        }
    }
}

/**
 * @brief 퀵 슬롯 데이터가 업데이트되었을 때 호출되는 콜백 함수입니다.
 */
void UNS_QuickSlotPanel::OnQuickSlotDataUpdated()
{
    if (QuickSlotComponent)
    {
        RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
    }
}