#include "UI/InGame/NS_NearbyItemEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "UI/InGame/NS_NearbyItemsWidget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "World/Pickup.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 버튼 이벤트 바인딩에 사용됩니다.
 */
void UNS_NearbyItemEntry::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("NS_NearbyItemEntry::NativeConstruct 호출"));
	
	// 위젯에 바인딩된 컴포넌트들이 유효한지 확인합니다.
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
	
	// PickupButton(아이템 획득 버튼)이 유효하면 클릭 이벤트를 바인딩합니다.
	if (PickupButton)
	{
		PickupButton->OnClicked.AddDynamic(this, &UNS_NearbyItemEntry::OnPickupButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PickupButton이 유효하지 않습니다. 블루프린트에서 이름이 정확한지 확인하세요."));
	}
}

/**
 * @brief 이 항목 위젯에 표시할 아이템 정보를 설정합니다.
 * @param InItemInfo 설정할 아이템 정보 구조체입니다.
 *        아이템 이름, 수량, 아이콘 등을 업데이트합니다.
 */
void UNS_NearbyItemEntry::SetItemInfo(const FNearbyItemInfo& InItemInfo)
{
	ItemInfo = InItemInfo;
	
	UE_LOG(LogTemp, Warning, TEXT("SetItemInfo 호출 - 아이템: %s"), 
		ItemInfo.Item ? *ItemInfo.Item->GetName() : TEXT("None"));
	
	// 위젯에 바인딩된 컴포넌트들이 유효한지 다시 확인합니다.
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
	
	// ItemNameText가 유효하고 아이템 정보가 있으면 아이템 이름을 설정합니다.
	if (ItemNameText && ItemInfo.Item)
	{
		FText ItemName = ItemInfo.Item->GetItemName();
		UE_LOG(LogTemp, Warning, TEXT("아이템 이름: %s"), *ItemName.ToString());
		ItemNameText->SetText(ItemName);
	}
	
	// ItemQuantityText가 유효하면 아이템 수량을 설정합니다.
	if (ItemQuantityText)
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 수량: %d"), ItemInfo.Quantity);
		ItemQuantityText->SetText(FText::AsNumber(ItemInfo.Quantity));
	}
	
	// ItemIcon이 유효하고 아이템 정보가 있으면 아이템 아이콘을 설정합니다.
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
			
			// 아이콘이 없을 경우 기본 회색 브러시를 설정합니다.
			FSlateBrush DefaultBrush;
			DefaultBrush.TintColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
			ItemIcon->SetBrush(DefaultBrush);
			
			// 아이템 데이터를 직접 확인하여 아이콘 유무를 로깅합니다.
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

/**
 * @brief 이 항목 위젯의 부모 위젯(UNS_NearbyItemsWidget)을 설정합니다.
 *        아이템 획득 요청 시 부모 위젯의 함수를 호출하기 위해 사용됩니다.
 * @param InParentWidget 부모 위젯에 대한 포인터입니다.
 */
void UNS_NearbyItemEntry::SetParentWidget(UNS_NearbyItemsWidget* InParentWidget)
{
	ParentWidget = InParentWidget;
}

/**
 * @brief 아이템 획득 버튼이 클릭되었을 때 호출되는 이벤트 핸들러 함수입니다.
 *        부모 위젯을 통해 아이템 획득을 요청합니다.
 */
void UNS_NearbyItemEntry::OnPickupButtonClicked()
{
	// ParentWidget가 유효하고 ItemInfo.ItemActor가 유효하면 부모 위젯에 아이템 획득을 요청합니다.
	if (ParentWidget && ItemInfo.ItemActor)
	{
		ParentWidget->RequestPickupItem(ItemInfo.ItemActor);
	}
}