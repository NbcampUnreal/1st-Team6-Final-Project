#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h" // 상호작용 컴포넌트 포함
#include "NS_NearbyItemsWidget.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UScrollBox;       // UMG 스크롤 박스 위젯
class UNS_NearbyItemEntry; // 주변 아이템 항목 위젯
class APickup;          // 픽업 가능한 아이템 액터

/**
 * @brief 캐릭터 주변의 아이템 목록을 표시하는 위젯입니다.
 *        플레이어가 상호작용할 수 있는 주변 아이템들을 스크롤 가능한 목록으로 보여줍니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NearbyItemsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */
	virtual void NativeConstruct() override;
	
	/**
	 * @brief 주변 아이템 목록을 업데이트하여 UI에 표시합니다.
	 * @param NearbyItems 주변에 있는 아이템들의 정보가 담긴 배열입니다.
	 *        이 함수는 스크롤 박스의 내용을 지우고 새로운 아이템 항목들을 추가합니다.
	 */
	void UpdateItemsList(const TArray<FNearbyItemInfo>& NearbyItems);
	
	/**
	 * @brief 특정 아이템을 획득하도록 요청합니다.
	 * @param ItemActor 획득할 아이템 액터에 대한 포인터입니다.
	 *        블루프린트에서 호출 가능하며, 주로 아이템 항목 위젯에서 클릭 이벤트 발생 시 사용됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Nearby Items")
	void RequestPickupItem(APickup* ItemActor);
	
protected:
	/**
	 * @brief 아이템 목록을 표시할 스크롤 박스 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ItemsScrollBox;
	
	/**
	 * @brief 주변 아이템 항목 위젯의 클래스입니다.
	 *        이 클래스를 기반으로 각 아이템에 대한 UI 항목을 생성합니다.
	 */	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_NearbyItemEntry> ItemEntryClass;
	
private:
	/**
	 * @brief 플레이어 캐릭터의 상호작용 컴포넌트 참조입니다.
	 *        아이템 획득 요청 등을 처리하기 위해 사용됩니다.
	 */
	UPROPERTY()
	UNS_InteractionComponent* InteractionComponent;
	
	/**
	 * @brief 플레이어 캐릭터에서 상호작용 컴포넌트를 찾아 할당하는 함수입니다.
	 *        NativeConstruct에서 호출되어 InteractionComponent를 초기화합니다.
	 */
	void FindInteractionComponent();
};