// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "NS_InventoryMainWidget.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class ANS_PlayerCharacterBase; // 플레이어 캐릭터 클래스
class UNS_NearbyItemsPanel; // 주변 아이템 위젯 클래스

/**
 * @brief 인벤토리 메인 메뉴를 나타내는 위젯입니다.
 *        주로 인벤토리 패널, 장비 슬롯 등 인벤토리 관련 UI를 포함하고 관리합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 이 인벤토리 메뉴와 연동되는 플레이어 캐릭터에 대한 참조입니다.
	 *        플레이어의 인벤토리 데이터를 가져오거나 조작하는 데 사용됩니다.
	 */
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;
	
	/**
	 * @brief 주변 아이템 위젯을 업데이트합니다.
	 * @param NearbyItems 주변에 있는 아이템들의 정보가 담긴 배열입니다.
	 */
	void UpdateNearbyItemsWidget(const TArray<FNearbyItemInfo>& NearbyItems);
	
	/**
	 * @brief 주변 아이템 위젯을 생성하고 표시합니다.
	 */
	void CreateAndShowNearbyItemsWidget();
	
	/**
	 * @brief 주변 아이템 위젯 클래스를 반환합니다.
	 */
	TSubclassOf<UNS_NearbyItemsPanel> GetNearbyItemsWidgetClass() const { return NearbyItemsWidgetClass; }

protected:
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
	 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
	 * @param InGeometry 위젯의 지오메트리 정보입니다.
	 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
	 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
	 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
	 *        주로 아이템을 인벤토리 외부로 버리거나 다른 슬롯으로 이동할 때 사용됩니다.
	 */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	/**
	 * @brief 주변 아이템 위젯 클래스 - 블루프린트에서 직접 설정
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_NearbyItemsPanel> NearbyItemsWidgetClass;
	
	/**
	 * @brief 주변 아이템 위젯 인스턴스
	 */
	UPROPERTY()
	UNS_NearbyItemsPanel* NearbyItemsWidget;
};