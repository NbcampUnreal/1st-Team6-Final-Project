// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "InventoryTooltip.generated.h"

class UTextBlock;
class UInventoryItemSlot;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* InventorySlotBeingHovered;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponType;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WeaponTypeHorizontal;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* DamageHorizontal;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStack;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* MaxStackHorizontal;


	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* SellValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackWeight;

protected:
	virtual void NativeConstruct() override;
};
