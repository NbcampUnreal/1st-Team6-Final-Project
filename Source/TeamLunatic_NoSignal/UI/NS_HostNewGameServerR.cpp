// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_HostNewGameServerR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Spacer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_SaveLoadHelper.h"
#include "UI/NS_UIManager.h"
#include "Components/ComboBoxString.h"
#include "UI/NS_MainMenu.h"
#include "UI/NS_AreYouSureMenu.h"



void UNS_HostNewGameServerR::NativeConstruct()
{
	Super::NativeConstruct();
    CreateServerButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnCreateServerButtonClicked);
}

void UNS_HostNewGameServerR::OnYesSelected()//덮어쓰기 메세지 동의선택
{
	const FString SaveName = GetSaveSlotName();
	NS_SaveLoadHelper::DeleteExistingSave(SaveName);
    AreYouSureMenu->HideWidget();
	StartGame();
}
void UNS_HostNewGameServerR::OnNoSelected() //덮허쓰기 취소 석택
{
    AreYouSureMenu->HideWidget();
}
FString UNS_HostNewGameServerR::GetSaveSlotName() const
{
    return SaveNameEntryBox ? SaveNameEntryBox->GetText().ToString() : TEXT("DefaultSlot");
}
void UNS_HostNewGameServerR::ShowConfirmationMenu()
{
	if (AreYouSureMenu)
        AreYouSureMenu->SetVisibility(ESlateVisibility::Visible);
}
void UNS_HostNewGameServerR::StartGame()
{
    const FString SlotName = GetSaveSlotName();
    FString SelectedLevelName = NS_SaveLoadHelper::GameLevelName;

    FPlayerSaveData PlayerData;
    PlayerData.PlayerName = SlotName;
    PlayerData.Health = 100.f;
    PlayerData.SavePosition = FVector(0, 0, 300);

    FLevelSaveData LevelData;
    LevelData.LevelName = SelectedLevelName;
    LevelData.TempClearKeyItemPosition = FVector(100, 200, 300);

    // 세션 정보
    FName SessionName = FName(*SlotName);
    int32 MaxPlayers = FCString::Atoi(*ComboBoxString->GetSelectedOption());

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->ShowWait();

        UE_LOG(LogTemp, Error, TEXT("=== HostNewGameServerR ShowWait 호출 - 비활성화됨 ==="));

        GI->SetGameModeType(EGameModeType::MultiPlayMode);

        //저장은 여기서 해도 괜찮지만, 실패 복구 고려 시 나중으로 미루는 것도 방법
        //NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

        GI->CreateDedicatedSessionViaHTTP(SessionName, MaxPlayers);

        // 로딩 스크린은 실제 게임 레벨로 이동할 때만 표시
        // 맵 이동 이후 자동으로 UI가 정리될 것이므로 따로 Hide 안 해도 됨
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 없음"));
    }
}

void UNS_HostNewGameServerR::OnCreateServerButtonClicked()
{
    StartGame();
    if(1)return;

    const FString SlotName = GetSaveSlotName();

    if (NS_SaveLoadHelper::FindExistingSave(SlotName))
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
            AreYouSureMenu->YesButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnYesSelected);
            AreYouSureMenu->NoButton->OnClicked.AddDynamic(this, &UNS_HostNewGameServerR::OnNoSelected);
            // UI에 확인창 띄우기
            ShowConfirmationMenu();
        }
        else
			UE_LOG(LogTemp, Warning, TEXT("AreYouSureMenu is null"));
    }
    else
        StartGame();
}

