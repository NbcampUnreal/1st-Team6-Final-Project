#include "UI/NS_HostNewGameServerR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Spacer.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_SaveLoadHelper.h"
#include "UI/NS_MainMenu.h"
#include "UI/NS_AreYouSureMenu.h"

void UNS_HostNewGameServerR::NativeConstruct()
{
    Super::NativeConstruct();
    if (CreateServerButton)
        CreateServerButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnCreateServerButtonClicked);
}

// 덮어쓰기 Yes 클릭 (저장 삭제 후 새로 시작)
void UNS_HostNewGameServerR::OnYesSelected()
{
    const FString SaveName = GetSaveSlotName();
    NS_SaveLoadHelper::DeleteExistingSave(SaveName);
    if (AreYouSureMenu)
        AreYouSureMenu->HideWidget();
    StartGame();
}

// 덮어쓰기 No 클릭 (취소)
void UNS_HostNewGameServerR::OnNoSelected()
{
    if (AreYouSureMenu)
        AreYouSureMenu->HideWidget();
}

// 세이브 슬롯명 가져오기
FString UNS_HostNewGameServerR::GetSaveSlotName() const
{
    return SaveNameEntryBox ? SaveNameEntryBox->GetText().ToString() : TEXT("DefaultSlot");
}

// 덮어쓰기 확인창
void UNS_HostNewGameServerR::ShowConfirmationMenu()
{
    if (AreYouSureMenu)
        AreYouSureMenu->SetVisibility(ESlateVisibility::Visible);
}

void UNS_HostNewGameServerR::StartGame()
{
    // 세이브 슬롯명 = 방 이름
    const FString RoomName = GetSaveSlotName();

    // 인원수 (콤보박스 사용시)
    int32 MaxPlayers = 4;
    if (ComboBoxString && ComboBoxString->GetSelectedOption().IsNumeric())
        MaxPlayers = FCString::Atoi(*ComboBoxString->GetSelectedOption());

    // 실제 전달되는 방 이름 로그로 출력!
    UE_LOG(LogTemp, Warning, TEXT("[HostNewGameServerR] CreateSession 호출: 방 이름 = %s, 인원수 = %d"), *RoomName, MaxPlayers);

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);
        GI->CreateSession(FName(*RoomName), MaxPlayers);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance를 찾을 수 없음"));
    }
}


// 서버생성 버튼 클릭
void UNS_HostNewGameServerR::OnCreateServerButtonClicked()
{
    const FString SlotName = GetSaveSlotName();

    // 세이브 중복 체크 및 덮어쓰기 확인
    if (NS_SaveLoadHelper::FindExistingSave(SlotName))
    {
        if (!AreYouSureMenu && MainMenu)
        {
            UNS_MasterMenuPanel* WidgetA = MainMenu->GetWidget(EWidgetToggleType::AreYouSureMenu);
            AreYouSureMenu = Cast<UNS_AreYouSureMenu>(WidgetA);
        }
        if (AreYouSureMenu)
        {
            AreYouSureMenu->YesButton->OnClicked.RemoveAll(this);
            AreYouSureMenu->NoButton->OnClicked.RemoveAll(this);
            AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnYesSelected);
            AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnNoSelected);
            ShowConfirmationMenu();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AreYouSureMenu is null"));
            StartGame();
        }
    }
    else
    {
        StartGame();
    }
}
