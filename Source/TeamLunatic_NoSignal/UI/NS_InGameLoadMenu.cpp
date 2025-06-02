#include "UI/NS_InGameLoadMenu.h"
#include "UI/NS_SaveGameMetaData.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_SaveGame.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_LoadGameMenuPanel.h"
#include "Components/VerticalBox.h"
#include "UI/NS_NewGameR.h"
#include "UI/NS_MenuButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_SaveLoadHelper.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameStartMenu.h"

UNS_InGameLoadMenu::UNS_InGameLoadMenu(const FObjectInitializer& ObjectInitializer)
{
    //if (!LoadGameDataElementClass)
    //{
    //    static ConstructorHelpers::FClassFinder<UNS_LoadGameMenuPanel> WBP_LoadGameMenuPanel(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1.BP_LoadGameMenuPanel_v1_C"));
    //    if (WBP_LoadGameMenuPanel.Succeeded())
    //        LoadGameDataElementClass = WBP_LoadGameMenuPanel.Class;
    //}
}

void UNS_InGameLoadMenu::NativeConstruct()
{
    Super::NativeConstruct();
    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어
    if (!LoadGameDataElementClass)
    {
        LoadGameDataElementClass = LoadClass<UNS_LoadGameMenuPanel>(
            nullptr, TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1.BP_LoadGameMenuPanel_v1_C"));
    }

    LoadSaveSlotsToUI();

    StartGameButton->RootButton->OnClicked.AddDynamic(this, &UNS_InGameLoadMenu::StartGame);
   // MainMenuButton->RootButton->OnClicked.AddDynamic(this, &UNS_InGameLoadMenu::ReturnMainMenu);
  //  MainMenuButton->SetVisibility(ESlateVisibility::Hidden);
}
void UNS_InGameLoadMenu::ShowWidget()
{
    Super::ShowWidget();
    LoadSaveSlotsToUI();
}
//void UNS_InGameLoadMenu::LoadGameWidgetOfGameOver()
//{
//    MainMenuButton->SetVisibility(ESlateVisibility::Visible);
//}
//void UNS_InGameLoadMenu::ReturnMainMenu()
//{
//    Cast<UNS_InGameStartMenu>(MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu))->On_MainMenuClicked();
//    //MainMenuButton->SetVisibility(ESlateVisibility::Visible);
//}
void UNS_InGameLoadMenu::StartGame()
{
    if (!SelectChildPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("선택된 슬롯이 없습니다."));
        return;
    }

    FString SlotName = SelectChildPanel->SaveNameText->GetText().ToString();

    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("해당 슬롯 %s 은 존재하지 않습니다."), *SlotName);
        return;
    }

    // 로드
    UNS_SaveGame* LoadedGame = Cast<UNS_SaveGame>(
        UGameplayStatics::LoadGameFromSlot(SlotName, 0));

    if (!LoadedGame)
    {
        UE_LOG(LogTemp, Error, TEXT("세이브 불러오기 실패: %s"), *SlotName);
        return;
    }

    // 저장된 레벨 이름 가져오기
    if (LoadedGame->LevelSaves.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("레벨 정보가 없습니다."));
        return;
    }
    //------------------------------------------------------------------------

    FString LevelName = LoadedGame->LevelSaves[0].LevelName;

    // GameInstance에 슬롯 이름 등록 (선택사항)
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::SinglePlayMode);

        UGameplayStatics::OpenLevel(this, FName(*LevelName));
    }
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
            UIManager->HideInGameMenuWidget(GetWorld());
    }
}
void UNS_InGameLoadMenu::OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChidPanel)
{
    SelectChildPanel = nullptr;
    SaveVerticalBox->RemoveChild(ChidPanel);
    if (ChidPanel)
    {
        FString SlotName = ChidPanel->SaveNameText->GetText().ToString();
        NS_SaveLoadHelper::DeleteExistingSave(SlotName);
    }
}
void UNS_InGameLoadMenu::OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChidPanel)
{
    SelectChildPanel = ChidPanel;
    for (int32 i = 0; i < SaveVerticalBox->GetChildrenCount(); ++i)
    {
        if (UNS_LoadGameMenuPanel* ChildPanel = Cast<UNS_LoadGameMenuPanel>(SaveVerticalBox->GetChildAt(i)))
        {
          //  if (ChildPanel != SelectChildPanel)
                ChildPanel->UnSelectedCheck();
        }
    }
    SelectChildPanel->SelectedCheck();
}
void UNS_InGameLoadMenu::LoadSaveSlotsToUI()
{
    //LoadGameDataElementClass = LoadClass<UNS_LoadGameMenuPanel>(nullptr, TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1.BP_LoadGameMenuPanel_v1_C"));

    UNS_SaveGameMetaData* MetaData = NS_SaveLoadHelper::LoadSaveMetaData();

    if (!MetaData || !LoadGameDataElementClass) return;

    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어

    for (const FSaveMetaData& Meta : MetaData->SaveMetaDataArray)
    {
        UNS_LoadGameMenuPanel* NewPanel = CreateWidget<UNS_LoadGameMenuPanel>(GetWorld(), LoadGameDataElementClass);
        if (NewPanel)
        {
            NewPanel->SetSlotInfo(Meta.SaveGameSlotName, Meta.LevelName, Meta.SaveTime);
            NewPanel->OnSlotClicked.AddDynamic(this, &UNS_InGameLoadMenu::OnClickedSelectChildPanel);
            NewPanel->OnDeleteSlotClicked.AddDynamic(this, &UNS_InGameLoadMenu::OnClickedDeleteSlot);
            SaveVerticalBox->AddChild(NewPanel);
        }
    }
}