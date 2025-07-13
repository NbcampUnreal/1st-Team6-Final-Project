#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h" // 상호작용 컴포넌트 포함 (FNearbyItemInfo 사용)
#include "NS_NearbyItemEntry.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UTextBlock;           // UMG 텍스트 블록 위젯
class UImage;               // UMG 이미지 위젯
class UButton;              // UMG 버튼 위젯
class UNS_NearbyItemsPanel; // 주변 아이템 목록 위젯 (부모 위젯)
class APickup;              // 픽업 가능한 아이템 액터

/**
 * @brief 주변 아이템 목록의 각 항목을 표시하는 위젯입니다.
 *        아이템의 이름, 수량, 아이콘, 그리고 획득 버튼을 포함합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NearbyItemEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 이 항목 위젯에 표시할 아이템 정보를 설정합니다.
	 * @param InItemInfo 설정할 아이템 정보 구조체입니다.
	 *        아이템 이름, 수량, 아이콘 등을 업데이트합니다.
	 */
	void SetItemInfo(const FNearbyItemInfo& InItemInfo);
	
	/**
	 * @brief 이 항목 위젯의 부모 위젯(UNS_NearbyItemsWidget)을 설정합니다.
	 *        아이템 획득 요청 시 부모 위젯의 함수를 호출하기 위해 사용됩니다.
	 * @param InParentWidget 부모 위젯에 대한 포인터입니다.
	 */
	void SetParentWidget(UNS_NearbyItemsPanel* InParentWidget);
	
protected:
	/**
	 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 버튼 이벤트 바인딩에 사용됩니다.
	 */
	virtual void NativeConstruct() override;
	
	/**
	 * @brief 아이템의 이름을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	
	/**
	 * @brief 아이템의 수량을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantityText;
	
	/**
	 * @brief 아이템의 아이콘을 표시하는 이미지 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	
	/**
	 * @brief 아이템을 획득하기 위한 버튼 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* PickupButton;
	
	/**
	 * @brief 아이템 획득 버튼이 클릭되었을 때 호출되는 이벤트 핸들러 함수입니다.
	 *        부모 위젯을 통해 아이템 획득을 요청합니다.
	 */
	UFUNCTION()
	void OnPickupButtonClicked();
	
private:
	/**
	 * @brief 이 항목 위젯이 나타내는 아이템의 정보입니다.
	 */
	FNearbyItemInfo ItemInfo;
	
	/**
	 * @brief 이 항목 위젯의 부모 위젯(UNS_NearbyItemsWidget)에 대한 참조입니다.
	 */
	UPROPERTY()
	UNS_NearbyItemsPanel* ParentWidget;
};