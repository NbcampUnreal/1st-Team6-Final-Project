// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_AreYouSureMenu.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AreYouSureMenu : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UBackgroundBlur* BackroundBlur;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Backround;

	UPROPERTY(meta = (BindWidget))
	class UButton* YesButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* NoButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningSubtitleText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningTitleText;
	
	//UPROPERTY(meta = (BindWidget))
	//class USizeBox* SizeBox;
	//UPROPERTY(meta = (BindWidget))
	//class UCanvasPanel* CanvasPanel;
	//UPROPERTY(meta = (BindWidget))
	//class UOverlay* Overlay;

	UFUNCTION()
	void OnClickedYesSelected();
	UFUNCTION()
	void OnClickedNoSelected();

protected:
	virtual void NativeConstruct() override;
};
