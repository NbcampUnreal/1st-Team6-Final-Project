// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotSlotWidget.generated.h"

class UNS_InventoryBaseItem;
class UImage;
class UTextBlock;
class UNS_QuickSlotKey;
class UDragItemVisual;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetAssignedItem(UNS_InventoryBaseItem* Item);

    FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }

    void SetSlotIndex(int32 InIndex);
    void UpdateSlotDisplay();

    UFUNCTION(BlueprintCallable)
    void ClearAssignedItem();

    UFUNCTION(BlueprintCallable)
    bool IsEmpty() const;

    FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

    void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);

    bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

    UNS_InventoryBaseItem* GetAssignedItem() const { return AssignedItem; }

    TSubclassOf<UDragItemVisual> DragItemVisualClass;
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
