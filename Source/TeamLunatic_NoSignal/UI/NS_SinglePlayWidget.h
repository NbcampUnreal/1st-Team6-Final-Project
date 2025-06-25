// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_SinglePlayWidget.generated.h"

//class UButton;
class UNS_MenuButtonWidget;
class UNS_BaseMainMenu;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SinglePlayWidget : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
protected:

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_NewGame;



    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_MainMenu;

   UFUNCTION()
   void OnNewGameClicked();
   UFUNCTION()
   void OnLoadGameClicked();
   UFUNCTION()
   void OnMainMenuClicked();
};
