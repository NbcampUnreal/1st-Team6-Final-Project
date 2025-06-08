#include "UI/NS_SinglePlayLoadGameR.h"
#include "UI/NS_SaveGameMetaData.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_SaveGame.h"
#include "UI/NS_CommonType.h"
//#include "UI/NS_MainMenu.h"
#include "UI/NS_LoadGameMenuPanel.h"
#include "Components/VerticalBox.h"
#include "UI/NS_NewGameR.h"
#include "UI/NS_MenuButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_SaveLoadHelper.h"

UNS_SinglePlayLoadGameR::UNS_SinglePlayLoadGameR(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UNS_LoadGameMenuPanel> WBP_LoadGameMenuPanel( TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1"));
    if (WBP_LoadGameMenuPanel.Succeeded())
        LoadGameDataElementClass = WBP_LoadGameMenuPanel.Class;
}
void UNS_SinglePlayLoadGameR::NativeConstruct()
{
	Super::NativeConstruct();
    LoadSaveSlotsToUI();

	NewGameButton->RootButton->OnClicked.AddDynamic(this, &UNS_SinglePlayLoadGameR::StartGame);
}
void UNS_SinglePlayLoadGameR::StartGame()
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

    //// GameInstance에 슬롯 이름 등록 (선택사항)
    //if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    //{
    //    GI->SetGameModeType(EGameModeType::SinglePlayMode);
    //   
    //    UGameplayStatics::OpenLevel(this, FName(*LevelName));
    //}

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        // 싱글 모드 전용 GameMode 지정
        FString SelectedLevelName = TEXT("/Game/Maps/MainWorld");
        FString GameModePath = TEXT("Game=/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C");
        UE_LOG(LogTemp, Warning, TEXT("[StartGame] Opening level with options: %s"), *GameModePath);
        UGameplayStatics::OpenLevel(this, FName(*SelectedLevelName), true, GameModePath);
    }
}
void UNS_SinglePlayLoadGameR::OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChidPanel)
{
    SelectChildPanel = nullptr;
	SaveVerticalBox->RemoveChild(ChidPanel);
	if (ChidPanel)
	{
        FString SlotName = ChidPanel->SaveNameText->GetText().ToString();
        NS_SaveLoadHelper::DeleteExistingSave(SlotName);
	}
}
void UNS_SinglePlayLoadGameR::OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChidPanel)
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
}
void UNS_SinglePlayLoadGameR::LoadSaveSlotsToUI()
{
	//LoadGameDataElementClass = LoadClass<UNS_LoadGameMenuPanel>(nullptr, TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_LoadGameMenuPanel_v1.BP_LoadGameMenuPanel_v1_C"));

    UNS_SaveGameMetaData* MetaData = NS_SaveLoadHelper::LoadSaveMetaData();

    if (!MetaData || !LoadGameDataElementClass) return;

    SaveVerticalBox->ClearChildren(); // 기존 UI 클리어

    for (const FSaveMetaData& Meta : MetaData->SaveMetaDataArray)
    {
        UNS_LoadGameMenuPanel* NewPanel = CreateWidget<UNS_LoadGameMenuPanel>( GetWorld(), LoadGameDataElementClass);
        if (NewPanel)
        {
            NewPanel->SetSlotInfo(Meta.SaveGameSlotName, Meta.LevelName, Meta.SaveTime);
			NewPanel->OnSlotClicked.AddDynamic(this, &UNS_SinglePlayLoadGameR::OnClickedSelectChildPanel);
            NewPanel->OnDeleteSlotClicked.AddDynamic(this, &UNS_SinglePlayLoadGameR::OnClickedDeleteSlot);
            SaveVerticalBox->AddChild(NewPanel);
        }
    }
}
