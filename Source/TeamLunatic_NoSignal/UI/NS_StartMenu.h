// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_BaseWidgetInterface.h"
#include "UI/NS_CommonType.h"
//#include "UI/NS_MenuButtonWidget.h"
#include "NS_StartMenu.generated.h"

class UButton;
class UNS_MenuButtonWidget;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_StartMenu : public UUserWidget , public INS_BaseWidgetInterface//public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

protected:

    UFUNCTION()
    void OnSinglePlayerClicked();
    UFUNCTION()
    void OnSettingsClicked();
    UFUNCTION()
    void OnQuitClicked();
    UFUNCTION()
    void OnMultiPlayerClicked();

    // 블루프린트에서 구현할 함수
  //  UFUNCTION(BlueprintImplementableEvent, Category = "UI")
   // void OnSinglePlayerButtonClicked();

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_SinglePlayer;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_Settings;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_Quit;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_MultiPlayer;

    //UFUNCTION(BlueprintImplementableEvent)
    //void SelectWidget(EWidgetToggleType ToggleType);
};
