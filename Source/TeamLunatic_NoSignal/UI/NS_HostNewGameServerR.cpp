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

    FString SelectedLevelName = NS_SaveLoadHelper::GameLevelName; //ComboBoxString->GetSelectedOption();

    //이부분  PlayerData.Health,SavePosition => "Game Level 기획Table" 같은거에 받아서 적용되어야함. 임시로 그냥 넣은거임.
    FPlayerSaveData PlayerData;
    PlayerData.PlayerName = SlotName;
    PlayerData.Health = 100.f;
    PlayerData.SavePosition = FVector(0, 0, 300);

    FLevelSaveData LevelData;
    LevelData.LevelName = SelectedLevelName;
    LevelData.TempClearKeyItemPosition = FVector(100, 200, 300); //임의의 아이템의 position 임시로 작성해본거임.변경 or 삭제해야함.
    //NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

    bool bIsLAN = false;
    if (CheckBox_UseLAN)
    {
        bIsLAN = CheckBox_UseLAN->IsChecked();
    }

    // 2. 세션 이름 정의 (슬롯 이름 사용)
    FName SessionName = FName(*SlotName);

    // 3. 최대 접속 인원
    int32 MaxPlayers = FCString::Atoi(*EditableTextBox_MaxPlayers->GetText().ToString());

    // 4. 세션 생성 요청
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        // CreateSession 성공 시 호출되는 델리게이트 바인딩먼저해야  CreateSession 성공하면 broadcast되서 lamda가 호출됨.
        GI->OnCreateSessionSuccess.AddLambda([this, SelectedLevelName, SlotName, PlayerData, LevelData, GI]()
            {
                UGameplayStatics::OpenLevel(this, FName(*SelectedLevelName), true);

                GI->SetGameModeType(EGameModeType::MultiPlayMode); // 게임 모드 타입 설정

                NS_SaveLoadHelper::SaveGame(SlotName, PlayerData, LevelData);

                MainMenu->GetWidget(EWidgetToggleType::HostServer)->HideSubMenuWidget();
                MainMenu->GetWidget(EWidgetToggleType::MultiPlayer)->HideSubMenuWidget();
            });
      
        GI->CreateSession(SessionName, bIsLAN, MaxPlayers);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 없음"));
}
void UNS_HostNewGameServerR::OnCreateServerButtonClicked()
{
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

