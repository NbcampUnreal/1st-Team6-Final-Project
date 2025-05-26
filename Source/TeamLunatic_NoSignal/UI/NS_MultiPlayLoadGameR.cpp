#include "UI/NS_MultiPlayLoadGameR.h"
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
#include "UI/NS_MainMenu.h"
#include "UI/NS_HostNewGameServerR.h"
#include "UI/NS_HostLoadGameServerR.h"

UNS_MultiPlayLoadGameR::UNS_MultiPlayLoadGameR(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UNS_LoadGameMenuPanel> WBP_LoadGameMenuPanel(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1"));
    if (WBP_LoadGameMenuPanel.Succeeded())
        LoadGameDataElementClass = WBP_LoadGameMenuPanel.Class;
}
void UNS_MultiPlayLoadGameR::NativeConstruct()
{
    Super::NativeConstruct();
    LoadSaveSlotsToUI();

    NewGameButton->RootButton->OnClicked.AddDynamic(this, &UNS_MultiPlayLoadGameR::NewGame);

}
void UNS_MultiPlayLoadGameR::Init(UNS_MainMenu* NsMainMenu)
{
	Super::Init(NsMainMenu);
	//SubMenus.Add(EWidgetToggleType::HostServer,MainMenu->GetWidget(EWidgetToggleType::HostServer));
    //SubMenus.Add(EWidgetToggleType::ServerBrowser, MainMenu->GetWidget(EWidgetToggleType::ServerBrowser));
	SubMenus.Add(EWidgetToggleType::HostNewGame, MainMenu->GetWidget(EWidgetToggleType::HostNewGame));
    SubMenus.Add(EWidgetToggleType::HostLoadGame, MainMenu->GetWidget(EWidgetToggleType::HostLoadGame));
}
void UNS_MultiPlayLoadGameR::NewGame()
{
    HideSubMenuWidget();
    HideWidget();
    MainMenu->GetWidget(EWidgetToggleType::HostNewGame)->ShowWidget();
}

void UNS_MultiPlayLoadGameR::OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChidPanel)
{
    SelectChildPanel = nullptr;
    SaveVerticalBox->RemoveChild(ChidPanel);
    if (ChidPanel)
    {
        FString SlotName = ChidPanel->SaveNameText->GetText().ToString();
        NS_SaveLoadHelper::DeleteExistingSave(SlotName);
    }
}
void UNS_MultiPlayLoadGameR::OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChidPanel)
{
    SelectChildPanel = ChidPanel;
    SelectChildPanel->SelectedCheck();

    for (int32 i = 0; i < SaveVerticalBox->GetChildrenCount(); ++i)
    {
        if (UNS_LoadGameMenuPanel* ChildPanel = Cast<UNS_LoadGameMenuPanel>(SaveVerticalBox->GetChildAt(i)))
        {
            if (ChildPanel != SelectChildPanel)
                ChildPanel->UnSelectedCheck();
        }
    }
    UNS_HostLoadGameServerR* HostLoadGameServer = Cast< UNS_HostLoadGameServerR>(MainMenu->GetWidget(EWidgetToggleType::HostLoadGame));
    if (HostLoadGameServer)
    {
        HideSubMenuWidget();
        HideWidget();
        HostLoadGameServer->LoadSlotName = SelectChildPanel->SaveNameText->GetText().ToString();
        HostLoadGameServer->ShowWidget();
	}
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HostNewGameServer is null"));
    }
}
void UNS_MultiPlayLoadGameR::LoadSaveSlotsToUI()
{
    UNS_SaveGameMetaData* MetaData = NS_SaveLoadHelper::LoadSaveMetaData();

    if (!MetaData || !LoadGameDataElementClass) return;

    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어

    for (const FSaveMetaData& Meta : MetaData->SaveMetaDataArray)
    {
        UNS_LoadGameMenuPanel* NewPanel = CreateWidget<UNS_LoadGameMenuPanel>(GetWorld(), LoadGameDataElementClass);
        if (NewPanel)
        {
            NewPanel->SetSlotInfo(Meta.SaveGameSlotName, Meta.LevelName, Meta.SaveTime);
            NewPanel->OnSlotClicked.AddDynamic(this, &UNS_MultiPlayLoadGameR::OnClickedSelectChildPanel);
            NewPanel->OnDeleteSlotClicked.AddDynamic(this, &UNS_MultiPlayLoadGameR::OnClickedDeleteSlot);
            SaveVerticalBox->AddChild(NewPanel);
        }
    }
}

