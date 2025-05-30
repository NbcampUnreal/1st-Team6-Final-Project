// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_SaveNameMenu.h"
#include "NS_SaveNameMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/NS_SaveLoadHelper.h"
#include "UI/NS_SaveGameMetaData.h"
#include "UI/NS_AreYouSureMenu.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameMenu.h"
#include "UI/NS_PopUpMsg.h"

void UNS_SaveNameMenu::NativeConstruct()
{
	Super::NativeConstruct();
    SaveButton->OnClicked.AddDynamic(this,&UNS_SaveNameMenu::OnClickedSave);
    CancelButton->OnClicked.AddDynamic(this, &UNS_SaveNameMenu::OnClickedCancel);
}

void UNS_SaveNameMenu::OnClickedSave()
{
	FString SaveName = SaveNameTextBox->GetText().ToString();
    if (NS_SaveLoadHelper::FindExistingSave(SaveName))
    {
        if (!AreYouSureMenu)
        {
            UNS_MasterMenuPanel* WidgetA = MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu);
            AreYouSureMenu = Cast<UNS_AreYouSureMenu>(WidgetA);
        }
        if (AreYouSureMenu)
        {
            AreYouSureMenu->YesButton->OnClicked.RemoveAll(this);
            AreYouSureMenu->NoButton->OnClicked.RemoveAll(this);
            AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_SaveNameMenu::OnYesSelectedAreYouSure);
            AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_SaveNameMenu::OnNoSelectedAreYouSure);
            // UI에 확인창 띄우기
            AreYouSureMenu->ShowWidgetD();
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("AreYouSureMenu is null"));
    }
    else
        NewSave();
}
void UNS_SaveNameMenu::OnClickedCancel()
{
    HideWidget();
}

void UNS_SaveNameMenu::OnYesSelectedAreYouSure()
{
    const FString SaveName = SaveNameTextBox->GetText().ToString();
    NS_SaveLoadHelper::DeleteExistingSave(SaveName);
    UNS_MasterMenuPanel* SavePopUp = MainMenu->GetWidget(EWidgetToggleType::PopUpMsg);
    SavePopUp->ShowWidgetD();
    AreYouSureMenu->HideWidget();
}
void UNS_SaveNameMenu::OnNoSelectedAreYouSure()
{
    AreYouSureMenu->HideWidget();
}

void UNS_SaveNameMenu::NewSave()
{
    const FString SlotName = SaveNameTextBox->GetText().ToString();

    FString SelectedLevelName = NS_SaveLoadHelper::GameLevelName;//ComboBoxString_mapName->GetSelectedOption();

    //이부분  PlayerData.Health,SavePosition => "Game Level 기획Table" 같은거에 받아서 적용되어야함. 임시로 그냥 넣은거임.
    FPlayerSaveData PlayerData;
    PlayerData.PlayerName = SlotName;
    PlayerData.Health = 100.f;
    PlayerData.SavePosition = FVector(0, 0, 300);

    FLevelSaveData LevelData;
    LevelData.LevelName = SelectedLevelName;
    LevelData.TempClearKeyItemPosition = FVector(100, 200, 300); //임의의 아이템의 position 임시로 작성해본거임.변경 or 삭제해야함.

    NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

    HideWidget();
    UNS_PopUpMsg* PopUp =  Cast<UNS_PopUpMsg>(MainMenu->GetWidget(EWidgetToggleType::PopUpMsg));
    PopUp->ShowWidgetD();
}
