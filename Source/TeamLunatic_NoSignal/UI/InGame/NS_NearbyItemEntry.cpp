#include "UI/InGame/NS_NearbyItemEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "UI/InGame/NS_NearbyItemsWidget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "World/Pickup.h"

void UNS_NearbyItemEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemEntry::NativeConstruct 호출"));
	
	// 위젯 유효성 확인
	if (!ItemNameText)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemNameText가 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	if (!ItemQuantityText)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemQuantityText가 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	if (!ItemIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemIcon이 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	// 버튼 클릭 이벤트 바인딩
	if (PickupButton)
	{
		PickupButton->OnClicked.AddDynamic(this, &UNS_NearbyItemEntry::OnPickupButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PickupButton이 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
}

void UNS_NearbyItemEntry::SetItemInfo(const FNearbyItemInfo& InItemInfo)
{
	ItemInfo = InItemInfo;
	
	UE_LOG(LogTemp, Warning, TEXT("SetItemInfo 호출 - 아이템: %s"), 
		ItemInfo.Item ? *ItemInfo.Item->GetName() : TEXT("None"));
	
	// 위젯 유효성 확인
	if (!ItemNameText)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemNameText가 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	if (!ItemQuantityText)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemQuantityText가 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	if (!ItemIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemIcon이 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
	
	// 아이템 이름 설정
	if (ItemNameText && ItemInfo.Item)
	{
		FText ItemName = ItemInfo.Item->GetItemName();
		UE_LOG(LogTemp, Warning, TEXT("아이템 이름: %s"), *ItemName.ToString());
		ItemNameText->SetText(ItemName);
	}
	
	// 아이템 수량 설정
	if (ItemQuantityText)
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 수량: %d"), ItemInfo.Quantity);
		ItemQuantityText->SetText(FText::AsNumber(ItemInfo.Quantity));
	}
	
	// 아이템 아이콘 설정
	if (ItemIcon && ItemInfo.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 아이콘 설정 시도 - 아이템: %s, RowName: %s"), 
			*ItemInfo.Item->GetName(), 
			*ItemInfo.Item->ItemDataRowName.ToString());
		
		UTexture2D* IconTexture = ItemInfo.Item->GetItemIcon();
		if (IconTexture)
		{
			UE_LOG(LogTemp, Warning, TEXT("아이콘 텍스처 설정 성공: %s"), *IconTexture->GetName());
			ItemIcon->SetBrushFromTexture(IconTexture);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("아이콘 텍스처가 null입니다! 아이템: %s, RowName: %s"), 
				*ItemInfo.Item->GetName(), 
				*ItemInfo.Item->ItemDataRowName.ToString());
			
			// 기본 아이콘 설정
			FSlateBrush DefaultBrush;
			DefaultBrush.TintColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
			ItemIcon->SetBrush(DefaultBrush);
			
			// 아이템 데이터 직접 확인
			const FNS_ItemDataStruct* ItemData = ItemInfo.Item->GetItemData();
			if (ItemData)
			{
				UE_LOG(LogTemp, Warning, TEXT("아이템 데이터 확인: 아이콘 %s"), 
					ItemData->ItemAssetData.Icon ? TEXT("있음") : TEXT("없음"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("아이템 데이터를 가져올 수 없음"));
			}
		}
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