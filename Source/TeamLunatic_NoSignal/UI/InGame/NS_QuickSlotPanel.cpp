#include "UI/InGame/NS_QuickSlotPanel.h"
#include "UI/InGame/NS_QuickSlotSlotWidget.h"
#include "Components/HorizontalBox.h"
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
 * @brief 퀵 슬롯 슬롯들을 초기화합니다.
 *        SlotBox에 SlotWidgetClass를 기반으로 슬롯 위젯들을 생성하고 추가합니다.
 */
void UNS_QuickSlotPanel::InitializeSlots()
{
    // 필요한 컴포넌트나 클래스가 유효하지 않으면 함수를 종료합니다.
    if (!SlotBox || !QuickSlotComponent || !SlotWidgetClass)
    {
        return;
    }

    // 기존 슬롯들을 모두 제거합니다.
    SlotBox->ClearChildren();

    // 퀵 슬롯 컴포넌트로부터 현재 퀵 슬롯 아이템 목록을 가져옵니다.
    const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots = QuickSlotComponent->GetQuickSlots();
    // 퀵 슬롯의 개수를 가져옵니다.
    const int32 SlotCount = QuickSlots.Num();

    // 퀵 슬롯 개수만큼 슬롯 위젯을 생성하고 HorizontalBox에 추가합니다.
    for (int32 i = 0; i < SlotCount; ++i)
    {
        // 새로운 퀵 슬롯 슬롯 위젯을 생성합니다.
        UNS_QuickSlotSlotWidget* NewSlot = CreateWidget<UNS_QuickSlotSlotWidget>(this, SlotWidgetClass);
        if (NewSlot)
        {
            // 슬롯의 인덱스를 설정하고 HorizontalBox에 추가합니다.
            NewSlot->SetSlotIndex(i);
            SlotBox->AddChildToHorizontalBox(NewSlot);
        }
    }
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
    
    // 다음 틱에 슬롯을 초기화하고 퀵 슬롯을 갱신합니다.
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

    // 바인딩 성공 시 재시도 횟수를 초기화합니다.
    RetryCount = 0;
}

/**
 * @brief 퀵 슬롯 컴포넌트에서 호출하여 UI를 갱신합니다.
 * @param QuickSlots 갱신할 퀵 슬롯 아이템들의 배열입니다.
 *        각 슬롯 위젯에 아이템 정보를 설정하여 UI를 업데이트합니다.
 */
void UNS_QuickSlotPanel::RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots)
{
    // SlotBox가 유효하지 않으면 함수를 종료합니다.
    if (!SlotBox)
    {
        return;
    }
    
    // SlotBox에 있는 자식 위젯의 개수를 가져옵니다.
    const int32 NumSlots = SlotBox->GetChildrenCount();

    // 각 슬롯 위젯을 순회하며 아이템 정보를 갱신합니다.
    for (int32 i = 0; i < NumSlots; ++i)
    {
        // 현재 인덱스의 자식 위젯을 가져옵니다.
        if (UWidget* Child = SlotBox->GetChildAt(i))
        {
            // 자식 위젯을 UNS_QuickSlotSlotWidget으로 캐스팅합니다.
            if (UNS_QuickSlotSlotWidget* QSlot = Cast<UNS_QuickSlotSlotWidget>(Child))
            {
                // 퀵 슬롯 배열에 유효한 인덱스이고 아이템이 존재하면 슬롯에 아이템을 설정합니다.
                if (QuickSlots.IsValidIndex(i) && QuickSlots[i])
                {
                    QSlot->SetAssignedItem(QuickSlots[i]->GetItemData(), QuickSlots[i]->GetQuantity());
                }
                // 아이템이 없으면 슬롯을 비웁니다.
                else
                {
                    QSlot->ClearAssignedItem();
                }
            }
        }
    }
}

/**
 * @brief 퀵 슬롯 데이터가 업데이트되었을 때 호출되는 콜백 함수입니다.
 *        주로 퀵 슬롯 컴포넌트의 델리게이트에 바인딩됩니다.
 */
void UNS_QuickSlotPanel::OnQuickSlotDataUpdated()
{
    // 퀵 슬롯 컴포넌트와 SlotBox가 유효하면 퀵 슬롯을 갱신합니다.
    if (QuickSlotComponent && SlotBox)
    {
        RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
    }
}