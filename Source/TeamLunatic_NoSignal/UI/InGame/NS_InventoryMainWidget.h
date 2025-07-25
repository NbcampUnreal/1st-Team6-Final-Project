// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_InventoryMainWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UNS_QuickSlotPanel;
class ANS_PlayerCharacterBase;
class UNS_StatusComponent;
class UNS_QuickSlotComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UNS_QuickSlotPanel* QuickSlotPanel;

private:
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	UPROPERTY()
	UNS_StatusComponent* StatusComponent;

	UPROPERTY()
	UNS_QuickSlotComponent* QuickSlotComponent;

	UFUNCTION()
	void OnQuickSlotUpdated();
};