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

    FName SessionName = FName(*LoadSlotName);
    int32 MaxPlayers = FCString::Atoi(*EditableTextBox_MaxPlayers->GetText().ToString());

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->SetGameModeType(EGameModeType::MultiPlayMode);
        GI->CreateDedicatedSessionViaHTTP(SessionName, MaxPlayers);

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 없음"));
    }
}
