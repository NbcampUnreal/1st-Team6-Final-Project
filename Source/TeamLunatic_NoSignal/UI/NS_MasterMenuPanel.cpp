// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MasterMenuPanel.h"
#include "NS_MainUiPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_BaseMainMenu.h"

void UNS_MasterMenuPanel::NativeConstruct()
{
    Super::NativeConstruct();

  //  bool bIsRightPosMotionStart = (MyToggleType > EWidgetToggleType::Settings);
 //   MotionID = bIsRightPosMotionStart ? 1 : 0;
}

void UNS_MasterMenuPanel::SetToggleType()
{
   bool bIsRightPosMotionStart = (MyToggleType > EWidgetToggleType::Settings);
   MotionID = bIsRightPosMotionStart ? 1 : 0;
}
/*
void UNS_MasterMenuPanel::SelectWidget(EWidgetToggleType ToggleType)
{
    switch (ToggleType)
    {
    case EWidgetToggleType::SinglePlayer:
        SinglePlayer();
        break;
    case EWidgetToggleType::MultiPlayer:
        MultiPlayer();
        break;
    case EWidgetToggleType::Settings:
        Settings();
        break;
    case EWidgetToggleType::Video:
        Video();
        break;
    case EWidgetToggleType::Game:
        Game();
        break;
    case EWidgetToggleType::Audio:
        Audio();
        break;
    case EWidgetToggleType::Controls:
        Controls();
        break;
    case EWidgetToggleType::Quit:
        Quit();
        break;
    case EWidgetToggleType::Back:
        Back();
        break;
    case EWidgetToggleType::MainMenu:
        MainMenu();
        break;
    case EWidgetToggleType::ServerBrowser:
        ServerBrowser();
        break;
    case EWidgetToggleType::HostServer:
        HostServer();
        break;
    case EWidgetToggleType::Disconnect:
        Disconnect();
        break;
    case EWidgetToggleType::Resume:
        Resume();
        break;
    case EWidgetToggleType::NewGame:
        NewGame();
        break;
    case EWidgetToggleType::LoadGame:
        LoadGame();
        break;
    case EWidgetToggleType::SaveGame:
        SaveGame();
        break;
    }
}
*/
void UNS_MasterMenuPanel::SinglePlayer()
{

}
void UNS_MasterMenuPanel::MultiPlayer()
{

}
void UNS_MasterMenuPanel::Settings()
{

}
void UNS_MasterMenuPanel::Video()
{

}
void UNS_MasterMenuPanel::Game()
{

}
void UNS_MasterMenuPanel::Audio()
{

}
void UNS_MasterMenuPanel::Controls()
{

}
void UNS_MasterMenuPanel::Quit()
{

}
void UNS_MasterMenuPanel::Back()
{

}
void UNS_MasterMenuPanel::MainMenu1()
{

}
void UNS_MasterMenuPanel::ServerBrowser()
{

}
void UNS_MasterMenuPanel::HostServer()
{

}
void UNS_MasterMenuPanel::Disconnect()
{

}
void UNS_MasterMenuPanel::Resume()
{

}
void UNS_MasterMenuPanel::NewGame()
{

}
void UNS_MasterMenuPanel::LoadGame()
{

}

void UNS_MasterMenuPanel::SaveGame()
{

}

void UNS_MasterMenuPanel::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);
    if (MotionID == 0) 
        MainMenu->PlayAnimationShowL();
    else
        MainMenu->PlayAnimationShowR();
}

void UNS_MasterMenuPanel::ShowWidgetD()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UNS_MasterMenuPanel::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UNS_MasterMenuPanel::HideSubMenuWidget()
{
    for (const TPair<EWidgetToggleType, UNS_MasterMenuPanel*>& Elem : SubMenus)
    {
        if (Elem.Value)
            Elem.Value->HideWidget();
    }
}

void UNS_MasterMenuPanel::Init(UNS_BaseMainMenu* NsMainMenu)
{
    MainMenu = NsMainMenu;
    SetToggleType();
}


void UNS_MasterMenuPanel::SetBlockInput(ANS_MainUiPlayerController* Player, bool bBlock)
{
    if (Player)
    {
        // 예시: 입력 블로킹을 여기서 세팅 (실제 구현은 프로젝트 구조에 따라 다름)
        Player->SetIgnoreMoveInput(bBlock);
        Player->SetIgnoreLookInput(bBlock);
    }
}

void UNS_MasterMenuPanel::PlayerInventoryComponentClose()
{
    // 인벤토리 닫는 실제 처리
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    ANS_MainUiPlayerController* MainUiPlayerController = Cast<ANS_MainUiPlayerController>(PC);
    if (!MainUiPlayerController) return;

    // 입력 해제
    SetBlockInput(MainUiPlayerController, false);

    // 위젯 제거
    //if (BP_PlayerInventory && BP_PlayerInventory->IsInViewport())
    //{
    //    BP_PlayerInventory->RemoveFromParent();
    //}

    // ESC 메뉴 포인터 초기화

  //  MainUiPlayerController->EscMenu = nullptr;

  //  UE_LOG(LogTemp, Log, TEXT("Closing inventory component..."));
    // 예: RemoveFromParent(), BlockInput = false, 참조 초기화 등
}
