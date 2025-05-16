// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_CommonType.h"
#include "NS_TheSettingsWidget.generated.h"

class UNS_MenuButtonWidget;
class UNS_MainMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_TheSettingsWidget : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void Init(UNS_MainMenu* NsMainMenu) override;
protected:

	UPROPERTY(meta = (BindWidget))
	UNS_MenuButtonWidget* BP_WidgetToggleButton_Game;

	UPROPERTY(meta = (BindWidget))
	UNS_MenuButtonWidget* BP_WidgetToggleButton_Video;

	UPROPERTY(meta = (BindWidget))
	UNS_MenuButtonWidget* BP_WidgetToggleButton_Audio;

	UPROPERTY(meta = (BindWidget))
	UNS_MenuButtonWidget* BP_WidgetToggleButton_Controls;

	UPROPERTY(meta = (BindWidget))
	UNS_MenuButtonWidget* BP_WidgetToggleButton_MainMenu;

	UFUNCTION()
	void OnGameClicked();

	UFUNCTION()
	void OnVideoClicked();

	UFUNCTION()
	void OnAudioClicked();

	UFUNCTION()
	void OnControlsClicked();

	UFUNCTION()
	void OnMainMenuClicked();
};
