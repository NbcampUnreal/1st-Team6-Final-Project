#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_OpenLevelMap.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_OpenLevelMap : public UUserWidget
{
	GENERATED_BODY()

public:
	// 플레이어 위치를 갱신할 이미지 위젯 참조
	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerLocationIcon;

	// 현재 캐릭터 위치 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Map")
	void UpdatePlayerLocation(FVector2D NormalizedPosition);

	// 지도 이미지 (디자이너에서 설정)
	UPROPERTY(EditAnywhere, Category = "UI")
	UTexture2D* MapImage;

	// 레벨의 실제 크기 (월드 좌표 기준)
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector2D LevelMinBound;
	
	UPROPERTY(EditAnywhere, Category = "Map")
	FVector2D LevelMaxBound;

	// 매 프레임 호출하여 위치 갱신
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// 위젯이 생성될 때 호출
	virtual void NativeConstruct() override;
};
