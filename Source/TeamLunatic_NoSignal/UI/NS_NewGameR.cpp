
#include "UI/NS_NewGameR.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
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
#include "UI/NS_UIManager.h"
//#include "GameFlow\NS_GameInstance.h"

void UNS_NewGameR::OnStartGameClicked()
{
    FString SaveName = GetSaveSlotName();

    StartGame();
    //밑에 코드는 세이브 중복 파일있으면 덮허쓰기 창 띄우는건데 NewGame 매뉴는 그냥 밀어버리는게 맥락상 맞을것같다. 밀어버리자.
     
    //if (NS_SaveLoadHelper::FindExistingSave(SaveName))
    //{
    //    if (!AreYouSureMenu)
    //    {
    //        UNS_MasterMenuPanel* WidgetA = MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu);
    //        AreYouSureMenu = Cast<UNS_AreYouSureMenu>(WidgetA);
    //    }
    //    if (AreYouSureMenu)
    //    {
    //        AreYouSureMenu->YesButton->OnClicked.RemoveAll(this);
    //        AreYouSureMenu->NoButton->OnClicked.RemoveAll(this);
    //        AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnYesSelected);
    //        AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnNoSelected);
    //        // UI에 확인창 띄우기
    //        ShowConfirmationMenu();
    //    }
    //    else
    //        UE_LOG(LogTemp, Warning, TEXT("AreYouSureMenu is null"));
    //}
    //else
    //    StartGame(); 
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

void UNS_NewGameR::NativeConstruct()
{
    Super::NativeConstruct();

	StartGameButton->OnClicked.AddDynamic(this, &UNS_NewGameR::OnStartGameClicked);

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
    FString SelectedLevelName = TEXT("/Game/Maps/MainWorld");

    FPlayerSaveData PlayerData;
    PlayerData.PlayerName = SlotName;
    PlayerData.Health = 100.f;
    PlayerData.SavePosition = FVector(0, 0, 300);

    FLevelSaveData LevelData;
    LevelData.LevelName = SelectedLevelName;
    LevelData.TempClearKeyItemPosition = FVector(100, 200, 300);

    NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetCurrentSaveSlot(SlotName);

        FString GameModePath = TEXT("Game=/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C");

        GI->GetUIManager()->LoadingScreen(GetWorld());

        GI->GetUIManager()->OnLoadingFinished.BindLambda([SelectedLevelName, GameModePath, GI]()
        {
            UGameplayStatics::OpenLevel(GI->GetWorld(), FName(*SelectedLevelName), true, GameModePath);
        });
    }
}


