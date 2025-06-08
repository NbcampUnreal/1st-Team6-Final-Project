// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotPanel.generated.h"

class UHorizontalBox;
class UNS_QuickSlotSlotWidget;
class UNS_InventoryBaseItem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotPanel : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void RefreshQuickSlot();
    bool IsItemAlreadyAssigned(UNS_InventoryBaseItem* Item) const;
    void AssignItemToSlot(int32 SlotIndex, UNS_InventoryBaseItem* Item);
    bool AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item);
    void UseSlot(int32 SlotIndex);
    UFUNCTION()
    void TryBindQuickSlot();
protected:
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SlotBox;

    UPROPERTY(EditDefaultsOnly, Category = "QuickSlot")
    TSubclassOf<UNS_QuickSlotSlotWidget> SlotWidgetClass;

    UPROPERTY()
    TArray<UNS_QuickSlotSlotWidget*> SlotWidgets;
};
