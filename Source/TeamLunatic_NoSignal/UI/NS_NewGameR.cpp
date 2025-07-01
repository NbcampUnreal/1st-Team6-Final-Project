
#include "UI/NS_NewGameR.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UI/NS_SaveGame.h"
#include "UI/NS_UIManager.h"
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

    FString GameModePath = TEXT("Game=/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C");
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        // 임시: UIManager 방식과 GameInstance 방식 둘 다 시도
        if (UNS_UIManager* UIManager = GI->GetUIManager())
        {
            UIManager->ShowLoadingScreen(GetWorld());
            UE_LOG(LogTemp, Warning, TEXT("StartGame: UIManager 로딩 스크린 표시"));
        }

        // GameInstance에서도 영구 로딩 스크린 생성
        GI->CreatePersistentLoadingScreen();
        UE_LOG(LogTemp, Warning, TEXT("StartGame: 영구 로딩 스크린 생성"));

        // 다음 틱에 실행 - 로딩 스크린 렌더링 완료 보장
        GetWorld()->GetTimerManager().SetTimerForNextTick([GI, SelectedLevelName, GameModePath]()
        {
            UE_LOG(LogTemp, Error, TEXT("=== 레벨 전환 시작 (다음 틱) ==="));
            UGameplayStatics::OpenLevel(GI->GetWorld(), FName(*SelectedLevelName), true, GameModePath);
        });
    }
}


FString UNS_NewGameR::GetSaveSlotName() const
{
    // 여기에 세이브 슬롯 이름을 반환하는 로직을 구현합니다.
    // 예를 들어, 다른 클래스에서 구현된 방식을 참고하면:
    
    // 1. 에디터블 텍스트 박스에서 이름을 가져오는 방식
    // return SaveNameEntryBox ? SaveNameEntryBox->GetText().ToString() : TEXT("DefaultSlot");
    
    // 2. 고정된 이름을 사용하는 방식
    return TEXT("NewGameSlot");
    
    // 3. 현재 시간을 포함한 이름을 생성하는 방식
    // FDateTime Now = FDateTime::Now();
    // return FString::Printf(TEXT("NewGame_%s"), *Now.ToString());
}


