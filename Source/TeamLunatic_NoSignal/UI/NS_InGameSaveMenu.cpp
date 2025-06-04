// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_InGameSaveMenu.h"
#include "UI/NS_SaveGameMetaData.h"
#include "UI/NS_SaveLoadHelper.h"
#include "UI/NS_LoadGameMenuPanel.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_AreYouSureMenu.h"
#include "Components/Button.h"   
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_SaveNameMenu.h"
#include "UI/NS_PopUpMsg.h"


void UNS_InGameSaveMenu::NativeConstruct()
{
	Super::NativeConstruct();

    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어

    //NS_SaveLoadHelper::FixSaveData();
    //NS_SaveLoadHelper::DeleteAllSaves();
}
void  UNS_InGameSaveMenu::Init(UNS_BaseMainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);

    UNS_MasterMenuPanel* NsSaveNameMenu = MainMenu->GetWidget(EWidgetToggleType::SaveNameMenu);
    SaveNameMenu = Cast<UNS_SaveNameMenu>(NsSaveNameMenu);
    NewSaveButton->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameSaveMenu::NewSave);

    UNS_PopUpMsg* PopUp = Cast<UNS_PopUpMsg>(MainMenu->GetWidget(EWidgetToggleType::PopUpMsg));
    PopUp->OnSuccessOkButtonEvent.AddDynamic(this, &UNS_InGameSaveMenu::LoadSaveSlotsToUI);
}
void UNS_InGameSaveMenu::ShowWidget()
{
    Super::ShowWidget();
    LoadSaveSlotsToUI();
}

void UNS_InGameSaveMenu::LoadSaveSlotsToUI()
{
    UNS_SaveGameMetaData* MetaData = NS_SaveLoadHelper::LoadSaveMetaData();

    if (!MetaData || !SaveGameDataElementClass) return;

    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어

    for (const FSaveMetaData& Meta : MetaData->SaveMetaDataArray)
    {
        UNS_LoadGameMenuPanel* NewPanel = CreateWidget<UNS_LoadGameMenuPanel>(GetWorld(), SaveGameDataElementClass);
        if (NewPanel)
        {
            NewPanel->SetSlotInfo(Meta.SaveGameSlotName, Meta.LevelName, Meta.SaveTime);
            NewPanel->OnSlotClicked.AddDynamic(this, &UNS_InGameSaveMenu::OnClickedSelectChildPanel);
            NewPanel->OnDeleteSlotClicked.AddDynamic(this, &UNS_InGameSaveMenu::OnClickedDeleteSlot);
            SaveVerticalBox->AddChild(NewPanel);
        }
    }
}
void UNS_InGameSaveMenu::OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChildPanel)
{
    SelectChildPanel = ChildPanel;
	
    if (!AreYouSureMenu)
    {
        UNS_MasterMenuPanel* WidgetA = MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu);
        AreYouSureMenu = Cast<UNS_AreYouSureMenu>(WidgetA);
    }
    if (AreYouSureMenu)
    {
        AreYouSureMenu->YesButton->OnClicked.RemoveAll(this);
        AreYouSureMenu->NoButton->OnClicked.RemoveAll(this);
        AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_InGameSaveMenu::OnYesSelected);
        AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_InGameSaveMenu::OnNoSelected);
        // UI에 확인창 띄우기
        AreYouSureMenu->ShowWidgetD();
       // MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu)->ShowWidgetD();
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("AreYouSureMenu is null"));
}

void UNS_InGameSaveMenu::OnYesSelected()
{
    const FString SaveName = SelectChildPanel->SaveNameText->GetText().ToString();

    if (NS_SaveLoadHelper::DeleteExistingSave(SaveName))
    {
        FString SelectedLevelName = NS_SaveLoadHelper::GameLevelName;//ComboBoxString_mapName->GetSelectedOption();

        //이부분  PlayerData.Health,SavePosition => "Game Level 기획Table" 같은거에 받아서 적용되어야함. 임시로 그냥 넣은거임.
        FPlayerSaveData PlayerData;
        PlayerData.PlayerName = SaveName;
        PlayerData.Health = 100.f;
        PlayerData.SavePosition = FVector(0, 0, 300);

        FLevelSaveData LevelData;
        LevelData.LevelName = SelectedLevelName;
        LevelData.TempClearKeyItemPosition = FVector(100, 200, 300); //임의의 아이템의 position 임시로 작성해본거임.변경 or 삭제해야함.

        NS_SaveLoadHelper::SaveGame(SaveName, PlayerData, LevelData);

        AreYouSureMenu->HideWidget();
        MainMenu->GetWidget(EWidgetToggleType::PopUpMsg)->ShowWidgetD();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Error!!! DeleteExistingSave / %s "), *SaveName);
    }

}
void UNS_InGameSaveMenu::OnNoSelected()
{
    AreYouSureMenu->HideWidget();
}
void UNS_InGameSaveMenu::NewSave()
{
    SaveNameMenu->ShowWidgetD();

    //const FString SlotName = GetSaveSlotName();
    //FString SelectedLevelName = NS_SaveLoadHelper::GameLevelName;//ComboBoxString_mapName->GetSelectedOption();

    ////이부분  PlayerData.Health,SavePosition => "Game Level 기획Table" 같은거에 받아서 적용되어야함. 임시로 그냥 넣은거임.
    //FPlayerSaveData PlayerData;
    //PlayerData.PlayerName = SlotName;
    //PlayerData.Health = 100.f;
    //PlayerData.SavePosition = FVector(0, 0, 300);

    //FLevelSaveData LevelData;
    //LevelData.LevelName = SelectedLevelName;
    //LevelData.TempClearKeyItemPosition = FVector(100, 200, 300); //임의의 아이템의 position 임시로 작성해본거임.변경 or 삭제해야함.

    //NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);
}
void UNS_InGameSaveMenu::OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChildPanel)
{
    SelectChildPanel = nullptr;
    SaveVerticalBox->RemoveChild(ChildPanel);
    if (ChildPanel)
    {
        FString SlotName = ChildPanel->SaveNameText->GetText().ToString();
        NS_SaveLoadHelper::DeleteExistingSave(SlotName);
    }
}