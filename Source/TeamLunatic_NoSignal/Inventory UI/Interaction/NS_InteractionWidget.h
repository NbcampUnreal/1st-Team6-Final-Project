// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_InteractionWidget.generated.h"

class ANS_PlayerCharacterBase;
struct FInteractableData;
class UTextBlock;
class UProgressBar;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	ANS_PlayerCharacterBase* PlayerCharacter;

	void UpdateWidget(const FInteractableData* InteractableData) const;

protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* QuantityText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UProgressBar* InteractionProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	float CurrentInteractionDuration;

	UFUNCTION(Category = "Interaction Widget | Interactable Data") 
	float UpdateInteractionProgress();

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
