#include "Inventory UI/NS_NearbyItemEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Inventory UI/NS_NearbyItemsWidget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "World/Pickup.h"

void UNS_NearbyItemEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 버튼 클릭 이벤트 바인딩
	if (PickupButton)
	{
		PickupButton->OnClicked.AddDynamic(this, &UNS_NearbyItemEntry::OnPickupButtonClicked);
	}
}

void UNS_NearbyItemEntry::SetItemInfo(const FNearbyItemInfo& InItemInfo)
{
	ItemInfo = InItemInfo;
	
	// 아이템 이름 설정
	if (ItemNameText && ItemInfo.Item)
	{
		ItemNameText->SetText(ItemInfo.Item->GetItemName());
	}
	
	// 아이템 수량 설정
	if (ItemQuantityText)
	{
		ItemQuantityText->SetText(FText::AsNumber(ItemInfo.Quantity));
	}
	
	// 아이템 아이콘 설정
	if (ItemIcon && ItemInfo.Item && ItemInfo.Item->GetItemIcon())
	{
		ItemIcon->SetBrushFromTexture(ItemInfo.Item->GetItemIcon());
	}
}

void UNS_NearbyItemEntry::SetParentWidget(UNS_NearbyItemsWidget* InParentWidget)
{
	ParentWidget = InParentWidget;
}

void UNS_NearbyItemEntry::OnPickupButtonClicked()
{
	// 부모 위젯이 있고 아이템 액터가 유효하면 아이템 획득 요청
	if (ParentWidget && ItemInfo.ItemActor)
	{
		ParentWidget->RequestPickupItem(ItemInfo.ItemActor);
	}
}