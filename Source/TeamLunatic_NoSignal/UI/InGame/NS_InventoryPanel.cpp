// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_InventoryPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_InventoryComponent.h"
#include "UI/InGame/NS_InventoryItemSlot.h"
#include "UI/InGame/NS_ItemDragDropOperation.h"
#include "Item/NS_InventoryBaseItem.h"

/**
 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
 */
void UNS_InventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

/**
 * @brief 위젯이 생성될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_InventoryPanel::NativeConstruct()
{
    Super::NativeConstruct();

    // NativeConstruct 시점에는 GetOwningPlayerPawn()이 유효할 가능성이 높으므로, 인벤토리 바인딩을 시도합니다.
    TryBindInventory();
}

/**
 * @brief 인벤토리 패널을 인벤토리 컴포넌트에 바인딩을 시도합니다.
 *        주로 플레이어 캐릭터의 인벤토리 컴포넌트를 찾아 연결합니다.
 */
void UNS_InventoryPanel::TryBindInventory()
{
    // 현재 위젯을 소유한 플레이어 폰을 ANS_PlayerCharacterBase로 캐스팅합니다.
    PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
    if (PlayerCharacter)
    {
        // 플레이어 캐릭터로부터 인벤토리 컴포넌트를 가져옵니다.
        InventoryReference = PlayerCharacter->GetInventory();
        if (InventoryReference)
        {
            // 인벤토리 참조가 유효하면 인벤토리를 새로고침하고 로그를 출력합니다.
            RefreshInventory();
            UE_LOG(LogTemp, Warning, TEXT("[UI-Bind] 성공: %s, 컨트롤러: %s"),
                *GetName(), *GetOwningPlayer()->GetName());
            // 인벤토리 업데이트 델리게이트에 RefreshInventory 함수를 바인딩하여 인벤토리 변경 시 UI를 갱신합니다.
            InventoryReference->OnInventoryUpdated.AddUObject(this, &UNS_InventoryPanel::RefreshInventory);
        }
        else
        {
            // 인벤토리 참조가 null인 경우 오류 로그를 출력합니다.
            UE_LOG(LogTemp, Error, TEXT("[UI-Bind] 실패: InventoryReference가 null입니다. 위젯: %s"), *GetName());
        }
    }
    else
    {
        // 플레이어 캐릭터가 null인 경우 경고 로그를 출력합니다.
        UE_LOG(LogTemp, Warning, TEXT("[UI-Bind] 실패: PlayerCharacter nullptr"));
    }
}

/**
 * @brief 인벤토리의 무게 및 용량 정보를 텍스트로 설정하는 함수입니다.
 *        RefreshInventory 함수에서 호출됩니다.
 */
void UNS_InventoryPanel::SetInfoText() const
{
    // 현재 인벤토리의 총 무게와 최대 무게를 가져와 문자열로 포맷합니다.
    const float TotalWeight = FMath::Abs(InventoryReference->GetInventoryTotalWeight());
    const float MaxWeight = FMath::Abs(InventoryReference->GetWeightCapacity());
    const FString WeightInfoValue = FString::Printf(TEXT("%.1f/%.1f"), TotalWeight, MaxWeight);

    // 현재 인벤토리의 아이템 개수와 최대 슬롯 용량을 가져와 문자열로 포맷합니다.
    const FString CapacityInfoValue{
        FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
        + FString::FromInt(InventoryReference->GetSlotsCapacity())
    };
 
    // WeightInfo와 CapacityInfo 텍스트 블록에 설정된 문자열을 표시합니다.
    WeightInfo->SetText(FText::FromString(WeightInfoValue));
    CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

/**
 * @brief 인벤토리 내용을 새로고침하는 함수입니다.
 *        인벤토리 컴포넌트의 아이템 목록을 기반으로 슬롯들을 업데이트합니다.
 */
void UNS_InventoryPanel::RefreshInventory()
{
    // 인벤토리 참조와 인벤토리 슬롯 클래스가 유효한지 확인합니다.
    if (InventoryReference && InventorySlotClass)
    {
        // 수량이 0인 아이템을 정리합니다.
        InventoryReference->CleanUpZeroQuantityItems();

        // 기존의 모든 인벤토리 슬롯 위젯을 제거합니다.
        InventoryPanel->ClearChildren();

        // 인벤토리 컴포넌트의 현재 아이템 목록을 가져옵니다.
        const auto& Contents = InventoryReference->GetInventoryContents();

        UE_LOG(LogTemp, Warning, TEXT("[UI] RefreshInventory 호출됨 - Contents.Num() = %d"), Contents.Num());

        // 각 인벤토리 아이템에 대해 슬롯 위젯을 생성하고 추가합니다.
        for (UNS_InventoryBaseItem* const& InventoryItem : InventoryReference->GetInventoryContents())
        {
            // 아이템이 유효하지 않거나 수량이 0 이하면 건너뜁니다.
            if (!IsValid(InventoryItem) || InventoryItem->GetQuantity() <= 0)
                continue;

            // 새로운 인벤토리 아이템 슬롯 위젯을 생성합니다.
            UNS_InventoryItemSlot* ItemSlot = CreateWidget<UNS_InventoryItemSlot>(this, InventorySlotClass);
            // 생성된 슬롯에 아이템 참조를 설정합니다.
            ItemSlot->SetItemReference(InventoryItem);

            // 인벤토리 패널(WrapBox)에 아이템 슬롯을 추가합니다.
            InventoryPanel->AddChildToWrapBox(ItemSlot);
            UE_LOG(LogTemp, Warning, TEXT("[UI] 슬롯 추가: %s | Row: %s | 아이콘: %s"),
                *InventoryItem->GetName(),
                *InventoryItem->ItemDataRowName.ToString(),
                InventoryItem->AssetData.Icon ? TEXT("O") : TEXT("X"));
            UE_LOG(LogTemp, Warning, TEXT("InventoryContent Num = %d"), InventoryReference->GetInventoryContents().Num());
        }
    }
    // 인벤토리 정보 텍스트를 업데이트합니다.
    SetInfoText();
}

/**
 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
 * @param InGeometry 위젯의 지오메트리 정보입니다.
 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
 *        주로 아이템을 인벤토리 내에서 이동하거나 인벤토리로 추가할 때 사용됩니다.
 */
bool UNS_InventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // 부모 클래스의 NativeOnDrop을 호출합니다.
    // Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    // 드래그 앤 드롭 작업 객체를 UItemDragDropOperation으로 캐스팅합니다.
    const UNS_ItemDragDropOperation* ItemDragDrop = Cast<UNS_ItemDragDropOperation>(InOperation);

    // 드래그된 아이템과 인벤토리 참조가 유효하면 드롭 이벤트를 처리합니다.
    if (ItemDragDrop->SourceItem && InventoryReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("Detected an Item drop on InventoryPanel."))

            return true; // 드롭 이벤트를 처리했음을 반환합니다.
    }

    return false; // 드롭 이벤트를 처리하지 않았음을 반환합니다.
}
