// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_InGameSaveMenu.generated.h"

//class UEditableTextBox;
//class USaveGame;
//class UNS_AreYouSureMenu;
//class UButton;
//class UComboBoxString;

class UScrollBox;
class UVerticalBox;
class UNS_LoadGameMenuPanel;
class UNS_AreYouSureMenu;
class UNS_MenuButtonWidget;
class UNS_SaveNameMenu;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameSaveMenu : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
	virtual void ShowWidget() override;

	UFUNCTION()
	void LoadSaveSlotsToUI();

	UFUNCTION()
	void OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChildPanel);
	UFUNCTION()
	void OnYesSelected();
	UFUNCTION()
	void OnNoSelected();
	UFUNCTION()
	void NewSave();

	UFUNCTION()
	void OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChildPanel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UNS_LoadGameMenuPanel> SaveGameDataElementClass;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UScrollBox* InputsBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UVerticalBox* SaveVerticalBox;

protected:
	UNS_LoadGameMenuPanel* SelectChildPanel;  //UNS_SaveGameMenuPanel

	UNS_AreYouSureMenu* AreYouSureMenu;
	UNS_SaveNameMenu* SaveNameMenu;


	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_MenuButtonWidget* NewSaveButton;
	
};
