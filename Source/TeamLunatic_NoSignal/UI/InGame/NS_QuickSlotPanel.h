// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotPanel.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UNS_QuickSlotBox; // 퀵 슬롯 개별 슬롯 위젯
class UNS_InventoryBaseItem;  // 인벤토리 기본 아이템 클래스
class UNS_QuickSlotComponent; // 퀵 슬롯 컴포넌트

/**
 * @brief 퀵 슬롯 패널을 나타내는 위젯입니다.
 *        여러 개의 퀵 슬롯 슬롯 위젯을 포함하며, 퀵 슬롯 컴포넌트와 연동하여 아이템 정보를 표시합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
     *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
     */
    virtual void NativeConstruct() override;
    
    /**
     * @brief 퀵 슬롯 패널을 퀵 슬롯 컴포넌트에 바인딩을 시도합니다.
     *        주로 플레이어 캐릭터의 퀵 슬롯 컴포넌트를 찾아 연결합니다.
     */
    void TryBindQuickSlotPanel();

    /**
     * @brief 퀵 슬롯 컴포넌트에서 호출하여 UI를 갱신합니다.
     * @param QuickSlots 갱신할 퀵 슬롯 아이템들의 배열입니다.
     *        각 슬롯 위젯에 아이템 정보를 설정하여 UI를 업데이트합니다.
     */
    void RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots);

    /**
     * @brief 퀵 슬롯 데이터가 업데이트되었을 때 호출되는 콜백 함수입니다.
     *        주로 퀵 슬롯 컴포넌트의 델리게이트에 바인딩됩니다.
     */
    void OnQuickSlotDataUpdated();

    /**
     * @brief 5개의 퀵 슬롯 슬롯 위젯들입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(meta = (BindWidget))
    UNS_QuickSlotBox* Slot0;
    
    UPROPERTY(meta = (BindWidget))
    UNS_QuickSlotBox* Slot1;
    
    UPROPERTY(meta = (BindWidget))
    UNS_QuickSlotBox* Slot2;
    
    UPROPERTY(meta = (BindWidget))
    UNS_QuickSlotBox* Slot3;
    
    UPROPERTY(meta = (BindWidget))
    UNS_QuickSlotBox* Slot4;



    /**
     * @brief 이 패널과 연동되는 퀵 슬롯 컴포넌트입니다.
     *        플레이어 캐릭터에서 찾아 할당됩니다.
     */
    UPROPERTY()
    UNS_QuickSlotComponent* QuickSlotComponent;

private:
    /**
     * @brief UI 바인딩 실패 시 재시도 횟수를 추적하는 변수입니다.
     *        최대 재시도 횟수를 초과하면 더 이상 바인딩을 시도하지 않습니다.
     */
    int32 RetryCount = 0;
};