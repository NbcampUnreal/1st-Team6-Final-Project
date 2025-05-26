// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_CommonType.h"
#include "NS_MasterMenuPanel.generated.h"

class ANS_MainUiPlayerController;
class UNS_BaseMainMenu;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MasterMenuPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//void SelectWidget(EWidgetToggleType ToggleType);
	//UPROPERTY(BlueprintReadWrite)
	//UUserWidget* MainMenuWidget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Widget")
	EWidgetToggleType MyToggleType;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetToggleType();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	virtual void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ShowWidgetD();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void HideWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void HideSubMenuWidget();

	virtual void Init(UNS_BaseMainMenu* NsMainMenu);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")//EditDefaultsOnly
		UNS_BaseMainMenu* MainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TMap< EWidgetToggleType, UNS_MasterMenuPanel*> SubMenus;//TArray<UNS_MasterMenuPanel*> SubMenus;

	//UPROPERTY(BlueprintReadWrite)
	//UUserWidget* EscMenu;

	//UPROPERTY(BlueprintReadWrite)
	//UUserWidget* BP_PlayerInventory;

	//UFUNCTION(BlueprintCallable, Category = "Widget")
	//virtual void HideSubMenuWidget();

private:
	int32 MotionID = -1;

	void SetBlockInput(ANS_MainUiPlayerController* Player, bool bBlock);
	void PlayerInventoryComponentClose();

	void SinglePlayer();
	void MultiPlayer();
	void Settings();
	void Video();
	void Game();
	void Audio();
	void Controls();
	void Quit();
	void Back();
	void MainMenu1();
	void ServerBrowser();
	void HostServer();
	void Disconnect();
	void Resume();
	void NewGame();
	void LoadGame();
	void SaveGame();
};
