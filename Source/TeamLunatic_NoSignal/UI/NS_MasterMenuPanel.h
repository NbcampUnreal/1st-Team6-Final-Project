// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_CommonType.h"
#include "NS_MasterMenuPanel.generated.h"

class ANS_MainUiPlayerController;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MasterMenuPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SelectWidget(EWidgetToggleType ToggleType);

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MainMenuWidget;

protected:
	
	//UPROPERTY(BlueprintReadWrite)
	//UUserWidget* EscMenu;

	//UPROPERTY(BlueprintReadWrite)
	//UUserWidget* BP_PlayerInventory;

private:
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
	void MainMenu();
	void ServerBrowser();
	void HostServer();
	void Disconnect();
	void Resume();
	void NewGame();
	void LoadGame();
	void SaveGame();
};
