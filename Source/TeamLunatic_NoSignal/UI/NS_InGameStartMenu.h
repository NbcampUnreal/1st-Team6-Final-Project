// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_CommonType.h"
#include "NS_InGameStartMenu.generated.h"


class UNS_MenuButtonWidget;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameStartMenu : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
    virtual void ShowWidget() override;

    UFUNCTION()
    void On_MainMenuClicked();
protected:

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Resume;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_SaveGame;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_LoadGame;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Settings;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_MainMenu;

    //UPROPERTY(meta = (BindWidget))
    //UNS_MenuButtonWidget* BP_Disconnect;

    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Quit;

    UFUNCTION(BlueprintCallable, Category = "Widget")
    void OnResumeClicked();
    UFUNCTION()
    void OnSaveGameClicked();
    UFUNCTION()
    void OnLoadGameClicked();
    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnDisconnectClicked();
    UFUNCTION()
    void OnQuitClicked();
};
