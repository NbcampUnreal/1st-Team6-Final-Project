// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LoadMenuGameOver.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameStartMenu.h"

void UNS_LoadMenuGameOver::NativeConstruct()
{
	Super::NativeConstruct();
	LoadSaveSlotsToUI();

	MainMenuButton->RootButton->OnClicked.AddDynamic(this, &UNS_LoadMenuGameOver::ReturnMainMenu);
}

void UNS_LoadMenuGameOver::LoadSaveSlotsToUI()
{
	Super::LoadSaveSlotsToUI();
}

void UNS_LoadMenuGameOver::Init(UNS_BaseMainMenu* NsMainMenu)
{
	Super::Init(NsMainMenu);
	SubMenus.Add(EWidgetToggleType::Settings, MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu));
}

void UNS_LoadMenuGameOver::ShowWidgetD()
{
	Super::ShowWidgetD();
	HideSubMenuWidget();
}

void UNS_LoadMenuGameOver::ReturnMainMenu()
{
	Cast<UNS_InGameStartMenu>(MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu))->On_MainMenuClicked();
	HideWidget();
}

