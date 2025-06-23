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
struct FNS_ItemDataStruct;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    void SetAssignedItem(const FNS_ItemDataStruct* ItemData, int32 Quantity);
    void SetSlotIndex(int32 Index);
    void ClearAssignedItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemIcon;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* AmountText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* AmmoText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UNS_QuickSlotKey* QuickSlotKey;

    UPROPERTY()
    UNS_InventoryBaseItem* AssignedItem;

    UPROPERTY()
    int32 SlotIndex;
};
