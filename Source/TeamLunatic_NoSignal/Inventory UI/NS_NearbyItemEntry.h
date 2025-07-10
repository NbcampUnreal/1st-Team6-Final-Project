#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "NS_NearbyItemEntry.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UNS_NearbyItemsWidget;
class APickup;

/**
 * 주변 아이템 목록의 각 항목을 표시하는 위젯
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NearbyItemEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 아이템 정보 설정
	void SetItemInfo(const FNearbyItemInfo& InItemInfo);
	
	// 부모 위젯 설정
	void SetParentWidget(UNS_NearbyItemsWidget* InParentWidget);
	
protected:
	// 위젯 초기화
	virtual void NativeConstruct() override;
	
	// 아이템 이름 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	
	// 아이템 수량 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantityText;
	
	// 아이템 아이콘
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	
	// 아이템 획득 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* PickupButton;
	
	// 아이템 획득 버튼 클릭 이벤트
	UFUNCTION()
	void OnPickupButtonClicked();
	
private:
	// 아이템 정보
	FNearbyItemInfo ItemInfo;
	
	// 부모 위젯
	UPROPERTY()
	UNS_NearbyItemsWidget* ParentWidget;
};