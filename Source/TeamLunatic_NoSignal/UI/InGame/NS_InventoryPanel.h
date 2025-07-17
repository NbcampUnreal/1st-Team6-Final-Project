// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "NS_InventoryPanel.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class ANS_PlayerCharacterBase; // 플레이어 캐릭터 클래스
class UNS_InventoryComponent; // 인벤토리 컴포넌트
class UNS_InventoryItemSlot;     // 인벤토리 아이템 슬롯 위젯

/**
 * @brief 플레이어의 인벤토리를 시각적으로 표시하는 위젯입니다.
 *        아이템 슬롯들을 포함하고, 인벤토리의 무게 및 용량 정보를 보여줍니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 인벤토리 내용을 새로고침하는 함수입니다.
	 *        인벤토리 컴포넌트의 아이템 목록을 기반으로 슬롯들을 업데이트합니다.
	 */
	UFUNCTION()
	void RefreshInventory();

	/**
	 * @brief 인벤토리 패널을 인벤토리 컴포넌트에 바인딩을 시도합니다.
	 *        주로 플레이어 캐릭터의 인벤토리 컴포넌트를 찾아 연결합니다.
	 */
	void TryBindInventory();

	/**
	 * @brief 아이템 슬롯들을 담는 WrapBox 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryPanel;

	/**
	 * @brief 현재 인벤토리의 총 무게를 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	/**
	 * @brief 인벤토리의 최대 용량을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	/**
	 * @brief 이 인벤토리 패널과 연동되는 플레이어 캐릭터에 대한 참조입니다.
	 */
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	/**
	 * @brief 이 인벤토리 패널과 연동되는 인벤토리 컴포넌트에 대한 참조입니다.
	 */
	UPROPERTY()
	UNS_InventoryComponent* InventoryReference;

	/**
	 * @brief 인벤토리 아이템 슬롯 위젯의 클래스입니다.
	 *        이 클래스를 기반으로 슬롯 위젯 인스턴스를 생성합니다.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNS_InventoryItemSlot> InventorySlotClass;

protected:
	/**
	 * @brief 인벤토리의 무게 및 용량 정보를 텍스트로 설정하는 함수입니다.
	 *        RefreshInventory 함수에서 호출됩니다.
	 */
	void SetInfoText() const;

	/**
	 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * @brief 위젯이 생성될 때 호출되는 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */
	void NativeConstruct();

	/**
	 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
	 * @param InGeometry 위젯의 지오메트리 정보입니다.
	 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
	 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
	 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
	 *        주로 아이템을 인벤토리 내에서 이동하거나 인벤토리로 추가할 때 사용됩니다.
	 */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
