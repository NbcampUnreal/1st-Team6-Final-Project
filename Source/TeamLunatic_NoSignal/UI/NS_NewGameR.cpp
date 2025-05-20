
#include "UI/NS_NewGameR.h"
#include "Kismet/GameplayStatics.h"
#include "UI/TempGameInstance.h"
#include "UI/NS_SaveGame.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MainMenu.h"
#include "UI/NS_SaveGameMetaData.h"
#include "UI/NS_AreYouSureMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Components/ComboBoxString.h"
#include "UI/NS_SaveLoadHelper.h"
//#include "GameFlow\NS_GameInstance.h"

void UNS_NewGameR::OnStartGameClicked()
{
    FString SaveName = GetSaveSlotName();

    if (NS_SaveLoadHelper::FindExistingSave(SaveName))
    {
        // UI에 확인창 띄우기
        ShowConfirmationMenu();
    }
    else
    {
        StartGame(); 
    }
}

void UNS_NewGameR::OnYesSelected()//덮어쓰기 메세지 동의선택
{
    const FString SaveName = GetSaveSlotName();

    NS_SaveLoadHelper::DeleteExistingSave(SaveName);
    
    HideConfirmationMenu();
    StartGame();
}

void UNS_NewGameR::OnNoSelected() //덮허쓰기 취소 석택
{
    HideConfirmationMenu();//창닫기
}
void UNS_NewGameR::Init(UNS_MainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);
    UNS_MasterMenuPanel* WidgetA = MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu);
    AreYouSureMenu = Cast<UNS_AreYouSureMenu>(WidgetA);
    if (AreYouSureMenu)
    {
        AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnYesSelected);
        AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnNoSelected);

        StartGameButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnStartGameClicked);
    }
}

void UNS_NewGameR::NativeConstruct()
{
    Super::NativeConstruct();
  // UGameplayStatics::DeleteGameInSlot("SaveGameMetaData", 0);

   
    //UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance());
    //if (GI)
    //    GI->SetGameModeType(EGameModeType::SingleplayMode);
    

    //UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance());
    //if (GI)
    //    GI->SetGameModeType(EGameModeType::MultiplayMode);

}

FString UNS_NewGameR::GetSaveSlotName() const
{
    return EditableTextBox_SaveName ? EditableTextBox_SaveName->GetText().ToString() : TEXT("DefaultSlot");
}

void UNS_NewGameR::ShowConfirmationMenu()
{
    if (AreYouSureMenu)
        AreYouSureMenu->SetVisibility(ESlateVisibility::Visible);
}

void UNS_NewGameR::HideConfirmationMenu()
{
    if (AreYouSureMenu)
        AreYouSureMenu->SetVisibility(ESlateVisibility::Hidden);
}
void UNS_NewGameR::StartGame()
{
    const FString SlotName = GetSaveSlotName();

    FString SelectedLevelName = ComboBoxString_mapName->GetSelectedOption();
	
    //이부분  PlayerData.Health,SavePosition => "Game Level 기획Table" 같은거에 받아서 적용되어야함. 임시로 그냥 넣은거임.
    FPlayerSaveData PlayerData;
    PlayerData.PlayerName = SlotName;
    PlayerData.Health = 100.f;
    PlayerData.SavePosition = FVector(0, 0, 300);

    FLevelSaveData LevelData;
    LevelData.LevelName = SelectedLevelName;
	LevelData.TempClearKeyItemPosition = FVector(100, 200, 300); //임의의 아이템의 position 임시로 작성해본거임.변경 or 삭제해야함.

    NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

    if (UTempGameInstance* GI = Cast<UTempGameInstance>(GetGameInstance()))
        GI->SetCurrentSaveSlot(SlotName);
    UGameplayStatics::OpenLevel(this, FName(SelectedLevelName));
}

