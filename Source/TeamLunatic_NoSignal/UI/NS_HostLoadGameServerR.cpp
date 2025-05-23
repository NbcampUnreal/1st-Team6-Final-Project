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
   FString LoadMapName ;

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
    bool bIsLAN = false;
    if (CheckBox_Use_LAN)
    {
        bIsLAN = CheckBox_Use_LAN->IsChecked();
    }

    // 2. 플레이어 컨트롤러 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 없음"));
        return;
    }

    // 3. 세션 생성 요청
    UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance());
    if (GI)
    {
        // CreateSession 성공 시 호출되는 델리게이트 바인딩먼저해야  CreateSession 성공하면 broadcast되서 lamda가 호출됨.
        GI->OnCreateSessionSuccess.AddLambda([this, LoadMapName, GI]()
        {
            UGameplayStatics::OpenLevel(this, FName(*LoadMapName), true);

            GI->SetGameModeType(EGameModeType::MultiPlayMode);// 게임 모드 타입 설정

            MainMenu->GetWidget(EWidgetToggleType::HostServer)->HideSubMenuWidget();
            MainMenu->GetWidget(EWidgetToggleType::MultiPlayer)->HideSubMenuWidget();
        });
        // 세션 생성
        int32 MaxPlayers = FCString::Atoi(*EditableTextBox_MaxPlayers->GetText().ToString());
        GI->CreateSession(PC, MaxPlayers, bIsLAN);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 없음"));
}
