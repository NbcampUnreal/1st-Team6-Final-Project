#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_LevelMapWidget.generated.h"

/**
 * @brief 게임 내 지도를 표시하고 플레이어의 위치를 지도 위에 업데이트하는 위젯입니다.
 *        주로 전체 맵을 열었을 때 사용됩니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LevelMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 플레이어의 현재 위치를 지도 위에 표시하는 이미지 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerLocationIcon;

	/**
	 * @brief 플레이어의 위치를 지도 위에서 업데이트하는 함수입니다.
	 * @param NormalizedPosition 정규화된(0.0 ~ 1.0) 플레이어의 지도 내 위치입니다.
	 *        이 값은 지도 이미지의 크기에 비례하여 실제 UI 좌표로 변환됩니다.
	 *        블루프린트에서 호출 가능합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Map")
	void UpdatePlayerLocation(FVector2D NormalizedPosition);

	/**
	 * @brief 지도 이미지로 사용될 텍스처입니다.
	 *        언리얼 에디터의 디테일 패널에서 설정됩니다.
	 */
	UPROPERTY(EditAnywhere, Category = "UI")
	UTexture2D* MapImage;

	/**
	 * @brief 레벨의 최소 경계(월드 좌표 기준)입니다.
	 *        지도의 정규화된 위치 계산에 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector2D LevelMinBound;
	
	/**
	 * @brief 레벨의 최대 경계(월드 좌표 기준)입니다.
	 *        지도의 정규화된 위치 계산에 사용됩니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector2D LevelMaxBound;

	/**
	 * @brief 매 프레임마다 호출되는 틱 함수입니다.
	 * @param MyGeometry 위젯의 지오메트리 정보입니다.
	 * @param InDeltaTime 마지막 프레임 이후 경과된 시간입니다.
	 *        주로 플레이어 위치를 지속적으로 갱신하는 데 사용됩니다.
	 */	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	/**
	 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */	virtual void NativeConstruct() override;
};
