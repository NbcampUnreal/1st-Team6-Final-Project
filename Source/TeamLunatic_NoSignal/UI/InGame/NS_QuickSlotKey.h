// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotKey.generated.h"

/**
 * @brief 퀵 슬롯에 할당된 키(예: 1, 2, 3 등)를 표시하는 위젯입니다.
 *        주로 UNS_QuickSlotSlotWidget 내부에 포함되어 사용됩니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotKey : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /**
     * @brief 이 퀵 슬롯 키 위젯이 나타낼 슬롯의 인덱스를 설정합니다.
     * @param Index 설정할 슬롯의 인덱스입니다. 이 인덱스는 화면에 표시될 키 번호로 사용됩니다.
     *        블루프린트에서 호출 가능합니다.
     */
    UFUNCTION(BlueprintCallable)
    void SetSlotIndex(int32 Index);

protected:
    /**
     * @brief 퀵 슬롯 키 번호를 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QSText;
};
