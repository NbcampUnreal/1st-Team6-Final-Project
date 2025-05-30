// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_PopUpMsg.generated.h"

class UButton;
class UTextBlock;
class UBorder;
class UNS_BaseMainMenu;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuccessOkButtonEvent);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PopUpMsg : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnYesButtonClicked();

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* YesButton;

	UTextBlock* SaveDescriptionText;
	UTextBlock* WarningTitleText;
	UBorder* Backround;

	UPROPERTY(BlueprintAssignable)
	FOnSuccessOkButtonEvent OnSuccessOkButtonEvent;
	
};
