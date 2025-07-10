#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "NS_NearbyItemsWidget.generated.h"

class UScrollBox;
class UNS_NearbyItemEntry;
class APickup;

/**
 * 캐릭터 주변의 아이템 목록을 표시하는 위젯
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NearbyItemsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 위젯 초기화
	virtual void NativeConstruct() override;
	
	// 주변 아이템 목록 업데이트
	void UpdateItemsList(const TArray<FNearbyItemInfo>& NearbyItems);
	
	// 아이템 획득 요청
	UFUNCTION(BlueprintCallable, Category = "Nearby Items")
	void RequestPickupItem(APickup* ItemActor);
	
protected:
	// 아이템 목록을 표시할 스크롤 박스
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ItemsScrollBox;
	
	// 아이템 항목 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_NearbyItemEntry> ItemEntryClass;
	
private:
	// 인터랙션 컴포넌트 참조
	UPROPERTY()
	UNS_InteractionComponent* InteractionComponent;
	
	// 인터랙션 컴포넌트 찾기
	void FindInteractionComponent();
};