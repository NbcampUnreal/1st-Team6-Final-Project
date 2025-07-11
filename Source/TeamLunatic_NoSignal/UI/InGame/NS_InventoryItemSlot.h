// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "NS_InventoryItemSlot.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UNS_InventoryTooltip;    // 인벤토리 툴팁 위젯
class UNS_DragItemVisual;      // 드래그 아이템 시각화 위젯
class UNS_InventoryBaseItem; // 인벤토리 기본 아이템 클래스

/**
 * @brief 인벤토리 내의 개별 아이템 슬롯을 나타내는 위젯입니다.
 *        아이템 아이콘, 수량, 그리고 드래그 앤 드롭 기능을 처리합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 이 슬롯에 할당된 아이템 참조를 설정합니다.
	 * @param ItemIn 설정할 UNS_InventoryBaseItem 객체에 대한 포인터입니다.
	 */
	FORCEINLINE void SetItemReference(UNS_InventoryBaseItem* ItemIn) { ItemReference = ItemIn; };

	/**
	 * @brief 이 슬롯에 할당된 아이템 참조를 반환합니다.
	 * @return UNS_InventoryBaseItem 객체에 대한 포인터입니다.
	 */
	FORCEINLINE UNS_InventoryBaseItem* GetItemReference() const { return ItemReference; };

protected:
	/**
	 * @brief 아이템 드래그 시 표시될 시각화 위젯의 클래스입니다.
	 *        블루프린트에서 설정됩니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UNS_DragItemVisual> DragItemVisualClass;

	/**
	 * @brief 아이템 툴팁 위젯의 클래스입니다.
	 *        블루프린트에서 설정됩니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UNS_InventoryTooltip> TooltipClass;

	/**
	 * @brief 이 슬롯에 현재 할당된 인벤토리 아이템에 대한 참조입니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot")
	UNS_InventoryBaseItem* ItemReference;

	/**
	 * @brief 아이템 슬롯의 시각적 테두리를 나타내는 Border 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UBorder* ItemBorder;

	/**
	 * @brief 아이템의 아이콘을 표시하는 Image 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UImage* ItemIcon;

	/**
	 * @brief 아이템의 수량을 표시하는 TextBlock 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UTextBlock* ItemQuantity;

	/**
	 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * @brief 위젯이 생성될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */
	virtual void NativeConstruct() override;

	/**
	 * @brief 마우스 버튼이 눌렸을 때 호출되는 이벤트 핸들러입니다.
	 *        드래그 시작 감지에 사용됩니다.
	 * @param InGeometry 위젯의 지오메트리 정보입니다.
	 * @param InMouseEvent 마우스 이벤트 정보입니다.
	 * @return 이벤트 처리 결과를 나타내는 FReply 객체입니다.
	 */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/**
	 * @brief 마우스 커서가 위젯 영역을 벗어났을 때 호출되는 이벤트 핸들러입니다.
	 *        툴팁 숨김 등에 사용됩니다.
	 * @param InMouseEvent 마우스 이벤트 정보입니다.
	 */
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/**
	 * @brief 드래그가 감지되었을 때 호출되는 이벤트 핸들러입니다.
	 *        드래그 앤 드롭 작업 객체를 생성하고 설정합니다.
	 * @param InGeometry 위젯의 지오메트리 정보입니다.
	 * @param InMouseEvent 마우스 이벤트 정보입니다.
	 * @param OutOperation 생성될 드래그 앤 드롭 작업 객체에 대한 참조입니다.
	 */
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	/**
	 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
	 *        아이템 이동 또는 교환 로직에 사용됩니다.
	 * @param InGeometry 위젯의 지오메트리 정보입니다.
	 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
	 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
	 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
	 */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
