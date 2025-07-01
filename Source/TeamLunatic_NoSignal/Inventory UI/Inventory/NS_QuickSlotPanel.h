// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotPanel.generated.h"

class UHorizontalBox;
class UNS_QuickSlotSlotWidget;
class UNS_InventoryBaseItem;
class UNS_QuickSlotComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    void InitializeSlots();
    void TryBindQuickSlotPanel();

    // 퀵슬롯 컴포넌트에서 호출하여 UI 갱신
    void RefreshQuickSlots(const TArray<TObjectPtr<UNS_InventoryBaseItem>>& QuickSlots);

    void OnQuickSlotDataUpdated();

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SlotBox;

    UPROPERTY(EditDefaultsOnly, Category = "QuickSlot")
    TSubclassOf<UNS_QuickSlotSlotWidget> SlotWidgetClass;
    UPROPERTY()
    UNS_QuickSlotComponent* QuickSlotComponent;

private:
    // 재시도 횟수 (UI 바인딩 실패 시 최대 재시도 횟수)
    int32 RetryCount = 0;
};