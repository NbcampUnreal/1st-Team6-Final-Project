// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "NS_DragItemVisual.generated.h"

/**
 * @brief 아이템을 드래그할 때 마우스 커서에 따라다니는 시각적 요소를 나타내는 위젯입니다.
 *        드래그 중인 아이템의 아이콘, 수량 등을 표시합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_DragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 드래그 중인 아이템의 테두리를 나타내는 Border 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	UBorder* ItemBorder;

	/**
	 * @brief 드래그 중인 아이템의 아이콘을 표시하는 Image 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	UImage* ItemIcon;

	/**
	 * @brief 드래그 중인 아이템의 수량을 표시하는 TextBlock 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	UTextBlock* ItemQuantity;
};