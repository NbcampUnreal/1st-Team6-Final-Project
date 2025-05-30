// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_HostLoadGameServerR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Spacer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_SaveLoadHelper.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MainMenu.h"

void UNS_HostLoadGameServerR::NativeConstruct()
{
	Super::NativeConstruct();

    CreateServerButton->OnClicked.AddDynamic(this, &UNS_HostLoadGameServerR::OnCreateServerButtonClicked);
}
void UNS_HostLoadGameServerR::OnCreateServerButtonClicked()
{
    FPlayerSaveData PlayerData;
    FLevelSaveData LevelData;
    FString LoadMapName;

    if (NS_SaveLoadHelper::LoadGame(LoadSlotName, PlayerData, LevelData))
    {
        LoadMapName = LevelData.LevelName;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("세이브 파일 로드 실패"));
        return;
    }

    // 1. LAN 체크박스 상태 확인
    // 2. 세션 이름 정의 (슬롯 이름 사용)
    FName SessionName = FName(*LoadSlotName);

    // 3. 최대 접속 인원
    int32 MaxPlayers = FCString::Atoi(*EditableTextBox_MaxPlayers->GetText().ToString());

    // 4. 세션 생성
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        // 세션 생성 성공 시 맵 이동 및 UI 정리
        GI->OnCreateSessionSuccess.AddLambda([this, LoadMapName, GI]()
            {
               // UGameplayStatics::OpenLevel(this, FName(*LoadMapName), true);
                GI->SetGameModeType(EGameModeType::MultiPlayMode);
                MainMenu->GetWidget(EWidgetToggleType::HostServer)->HideSubMenuWidget();
                MainMenu->GetWidget(EWidgetToggleType::MultiPlayer)->HideSubMenuWidget();
            });

        GI->CreateDedicatedSessionViaHTTP(SessionName, MaxPlayers);
    }
    else
    {

        UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 없음"));
    }
}