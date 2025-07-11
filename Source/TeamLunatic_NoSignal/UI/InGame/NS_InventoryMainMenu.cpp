// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_InventoryMainMenu.h"
#include "UI/InGame/NS_ItemDragDropOperation.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"

/**
 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
 */
void UNS_InventoryMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

/**
 * @brief 위젯이 생성될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_InventoryMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯을 소유한 플레이어 폰을 ANS_PlayerCharacterBase로 캐스팅하여 PlayerCharacter에 할당합니다.
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
}

/**
 * @brief 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수입니다.
 * @param InGeometry 위젯의 지오메트리 정보입니다.
 * @param InDragDropEvent 드래그 앤 드롭 이벤트 정보입니다.
 * @param InOperation 현재 진행 중인 드래그 앤 드롭 작업 객체입니다.
 * @return 드롭 이벤트가 처리되었으면 true, 그렇지 않으면 false를 반환합니다.
 *        주로 아이템을 인벤토리 외부로 버리거나 다른 슬롯으로 이동할 때 사용됩니다.
 */
bool UNS_InventoryMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// 드래그 앤 드롭 작업 객체를 UItemDragDropOperation으로 캐스팅합니다.
	const UNS_ItemDragDropOperation* ItemDragDrop = Cast<UNS_ItemDragDropOperation>(InOperation);

	// 플레이어 캐릭터가 유효하고 드래그된 아이템이 유효하면 아이템을 버립니다.
	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		// 플레이어 캐릭터의 DropItem 함수를 호출하여 아이템을 월드에 드롭합니다.
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true; // 드롭 이벤트를 처리했음을 반환합니다.
	}
	return false; // 드롭 이벤트를 처리하지 않았음을 반환합니다.
}
