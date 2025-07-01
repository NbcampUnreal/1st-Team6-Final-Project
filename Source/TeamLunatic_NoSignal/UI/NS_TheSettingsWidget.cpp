// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_TheSettingsWidget.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/TempGameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"

void UNS_TheSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
  /*  if (BP_WidgetToggleButton_Game)
        BP_WidgetToggleButton_Game->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_TheSettingsWidget::OnGameClicked);*/

    if (BP_WidgetToggleButton_Video)
        BP_WidgetToggleButton_Video->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_TheSettingsWidget::OnVideoClicked);
    

    if (BP_WidgetToggleButton_Audio)
        BP_WidgetToggleButton_Audio->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_TheSettingsWidget::OnAudioClicked);

    //if (BP_WidgetToggleButton_Controls)
    //    BP_WidgetToggleButton_Controls->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_TheSettingsWidget::OnControlsClicked);

    if (BP_WidgetToggleButton_MainMenu)
        BP_WidgetToggleButton_MainMenu->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_TheSettingsWidget::OnMainMenuClicked);
}

void UNS_TheSettingsWidget::Init(UNS_BaseMainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);

    //UE_LOG(LogTemp, Warning, TEXT("UNS_SinglePlayWidget NativeConstruct"));
  //  SubMenus.Add(EWidgetToggleType::Game, MainMenu->GetWidget(EWidgetToggleType::Game));
    SubMenus.Add(EWidgetToggleType::Video, MainMenu->GetWidget(EWidgetToggleType::Video));
    SubMenus.Add(EWidgetToggleType::Audio, MainMenu->GetWidget(EWidgetToggleType::Audio));
  //  SubMenus.Add(EWidgetToggleType::Controls, MainMenu->GetWidget(EWidgetToggleType::Controls));
}

void UNS_TheSettingsWidget::OnGameClicked()
{
    //HideSubMenuWidget();
    //if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::Game))
    //    Widget->ShowWidget();
}

void UNS_TheSettingsWidget::OnVideoClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::Video))
        Widget->ShowWidget();
}

void UNS_TheSettingsWidget::OnAudioClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::Audio))
        Widget->ShowWidget();
}

void UNS_TheSettingsWidget::OnControlsClicked()
{
    //HideSubMenuWidget();
    //if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::Controls))
    //    Widget->ShowWidget();
}

void UNS_TheSettingsWidget::OnMainMenuClicked()
{
    HideSubMenuWidget();
    HideWidget();

	if ( Cast<UNS_MainMenu>(MainMenu) )
        MainMenu->GetWidget(EWidgetToggleType::MainMenu)->ShowWidget();
    else
        MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu)->ShowWidget();
}
