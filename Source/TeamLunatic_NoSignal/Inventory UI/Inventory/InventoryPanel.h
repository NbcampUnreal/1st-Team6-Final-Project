// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "InventoryPanel.generated.h"


class ANS_PlayerCharacterBase;
class UInventoryComponent;
class UInventoryItemSlot;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

protected:
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	void NativeConstruct();
	void TryBindInventory();
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
