#include "UI/InGame/NS_InventoryMainWidget.h"
#include "UI/InGame/NS_ItemDragDropOperation.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "UI/InGame/NS_NearbyItemsPanel.h"
#include "Character/Components/NS_InteractionComponent.h"


void UNS_InventoryMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

// 위젯이 생성될 때 호출되는 함수
void UNS_InventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
}

/** 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수 */
bool UNS_InventoryMainWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// 드래그 앤 드롭 작업 객체를 UItemDragDropOperation으로 캐스팅
	const UNS_ItemDragDropOperation* ItemDragDrop = Cast<UNS_ItemDragDropOperation>(InOperation);

	// 플레이어 캐릭터가 유효하고 드래그된 아이템이 유효하면 아이템을 버림
	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		// 플레이어 캐릭터의 DropItem 함수를 호출하여 아이템을 월드에 드롭
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		
		return true; // 드롭 이벤트를 처리했음을 반환
	}
	return false; // 드롭 이벤트를 처리하지 않았음을 반환
}


