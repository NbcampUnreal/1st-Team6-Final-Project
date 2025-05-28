// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_SaveNameMenu.generated.h"

class UNS_AreYouSureMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SaveNameMenu : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UBorder* Backround;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UButton* SaveButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* WarningDescriptionText;

	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//class UTextBlock* WarningSubtitleText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* WarningTitleText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UEditableTextBox* SaveNameTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PopUp")
	UNS_AreYouSureMenu* AreYouSureMenu;



	UFUNCTION()
	void OnClickedSave();
	UFUNCTION()
	void OnYesSelectedAreYouSure();
	UFUNCTION()
	void OnNoSelectedAreYouSure();
	UFUNCTION()
	void OnClickedCancel();
	UFUNCTION()
	void NewSave();

protected:
	
};
