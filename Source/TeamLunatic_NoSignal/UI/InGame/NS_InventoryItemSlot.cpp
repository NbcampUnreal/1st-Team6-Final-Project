// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_InventoryItemSlot.h"
#include "UI/InGame/NS_InventoryTooltip.h"
#include "Item/NS_InventoryBaseItem.h"
#include "UI/InGame/NS_DragItemVisual.h"
#include "UI/InGame/NS_ItemDragDropOperation.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFlow/NS_GameInstance.h"

/**
 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
 */
void UNS_InventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 툴팁 클래스가 유효하면 툴팁 위젯을 생성하고 이 슬롯을 툴팁의 참조로 설정합니다.
	if (TooltipClass)
	{
		UNS_InventoryTooltip* Tooltip = CreateWidget<UNS_InventoryTooltip>(this, TooltipClass);
		Tooltip->InventorySlotBeingHovered = this;
		SetToolTip(Tooltip);
	}
}

/**
 * @brief 위젯이 생성될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_InventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	// ItemReference(아이템 참조)가 유효하면 아이템 정보에 따라 UI를 업데이트합니다.
	if (ItemReference)
	{
		// 아이템 타입에 따라 슬롯의 테두리 색상을 설정합니다.
		switch (ItemReference->ItemType)
		{
			// 소모품
		case EItemType::Consumable:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			// 엔딩 조건
		case EItemType::EndingTrigger:
			ItemBorder->SetBrushColor(FLinearColor::Red);
			break;
			// 장비
		case EItemType::Equipment:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			// 제작 재료
		case EItemType::Material:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			// 치료품
		case EItemType::Medical:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
			// 기타
		case EItemType::Misc:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			// 도구
		case EItemType::Utility:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		default:;
		}
		// 아이템 아이콘을 설정합니다.
		ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);

		// 아이템이 스택 가능한 경우 수량을 표시하고, 그렇지 않으면 수량 텍스트를 숨깁니다.
		if (ItemReference->NumericData.isStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

/**
 * @brief 마우스 버튼이 눌렸을 때 호출되는 이벤트 핸들러입니다.
 *        드래그 시작 감지에 사용됩니다.
 * @param InGeometry 위젯의 지오메트리 정보입니다.
 * @param InMouseEvent 마우스 이벤트 정보입니다.
 * @return 이벤트 처리 결과를 나타내는 FReply 객체입니다.
 */
FReply UNS_InventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// 좌클릭인 경우 드래그를 감지합니다.
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	// 우클릭인 경우 아이템 사용을 시도합니다.
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// ItemReference(아이템 참조)가 유효하면 아이템 사용을 시도합니다.
		if (ItemReference)
		{
			// 현재 위젯을 소유한 플레이어 폰을 ANS_PlayerCharacterBase로 캐스팅합니다.
			if (auto* Player = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn()))
			{
				UE_LOG(LogTemp, Warning, TEXT("[Client] 우클릭 - 서버에 아이템 사용 요청: %s"), *ItemReference->GetName());
				// 서버에 아이템 사용을 요청합니다.
				Player->Server_UseInventoryItem(ItemReference->ItemDataRowName); // 서버에 요청	
			}
		}
		return Reply.Handled(); // 이벤트를 처리했음을 반환합니다.
	}

	return Reply.Unhandled(); // 이벤트를 처리하지 않았음을 반환합니다.
}

/**
 * @brief 마우스 커서가 위젯 영역을 벗어났을 때 호출되는 이벤트 핸들러입니다.
 *        툴팁 숨김 등에 사용됩니다.
 * @param InMouseEvent 마우스 이벤트 정보입니다.
 */
void UNS_InventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

/**
 * @brief 드래그가 감지되었을 때 호출되는 이벤트 핸들러입니다.
 *        드래그 앤 드롭 작업 객체를 생성하고 설정합니다.
 * @param InGeometry 위젯의 지오메트리 정보입니다.
 * @param InMouseEvent 마우스 이벤트 정보입니다.
 * @param OutOperation 생성될 드래그 앤 드롭 작업 객체에 대한 참조입니다.
 */
void UNS_InventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// DragItemVisualClass(드래그 시각화 위젯 클래스)가 유효하면 드래그 시각화 및 드래그 앤 드롭 작업 객체를 생성합니다.
	if (DragItemVisualClass)
	{
		// 드래그 시각화 위젯을 생성하고 아이템 정보로 업데이트합니다.
		const TObjectPtr<UNS_DragItemVisual> DragVisual = CreateWidget<UNS_DragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));

		// 아이템 드래그 앤 드롭 작업 객체를 생성하고 소스 아이템과 인벤토리를 설정합니다.
		UNS_ItemDragDropOperation* DragItemOperation = NewObject<UNS_ItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwingInventory;

		// 드래그 시각화 위젯과 피벗을 설정합니다.
		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		// 생성된 드래그 앤 드롭 작업 객체를 OutOperation에 할당합니다.
		OutOperation = DragItemOperation;
	}
}

/**
 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
 *        아이템 이동 또는 교환 로직에 사용됩니다.
 * @param InGeometry 위젯의 지오메트리 정보입니다.
 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
 */
bool UNS_InventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 부모 클래스의 NativeOnDrop을 호출합니다.
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
