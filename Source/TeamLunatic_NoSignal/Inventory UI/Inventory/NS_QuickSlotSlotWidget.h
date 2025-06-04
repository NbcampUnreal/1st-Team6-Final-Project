// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotSlotWidget.generated.h"

class UNS_InventoryBaseItem;
class UImage;
class UTextBlock;
class UNS_QuickSlotKey;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetAssignedItem(UNS_InventoryBaseItem* Item);

    UFUNCTION(BlueprintCallable)
    void UseAssignedItem();

    FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }

    void SetSlotIndex(int32 InIndex);
    void UpdateSlotDisplay();

    UFUNCTION(BlueprintCallable)
    void ClearAssignedItem();

    UFUNCTION(BlueprintCallable)
    bool IsEmpty() const;

    UNS_InventoryBaseItem* GetAssignedItem() const { return AssignedItem; }
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemIcon;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* AmountText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UNS_QuickSlotKey* QuickSlotKey;

    UPROPERTY()
    UNS_InventoryBaseItem* AssignedItem;

    UPROPERTY()
    int32 SlotIndex;
};
