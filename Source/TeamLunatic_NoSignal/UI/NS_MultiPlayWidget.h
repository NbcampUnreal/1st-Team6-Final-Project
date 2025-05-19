// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_MultiPlayWidget.generated.h"

class UNS_MenuButtonWidget;
class UNS_MainMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MultiPlayWidget : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
    virtual void Init(UNS_MainMenu* NsMainMenu);

protected:
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_HostServer;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_ServerBrowser;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_WidgetToggleButton_MainMenu;

    UFUNCTION()
    void OnHostServerClicked();

    UFUNCTION()
    void OnServerBrowserClicked();

    UFUNCTION()
    void OnMainMenuClicked();
};
