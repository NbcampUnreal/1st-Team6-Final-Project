// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MainMenu.h"
#include "UI/NS_SinglePlayWidget.h"
#include "UI/NS_TheSettingsWidget.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetSystemLibrary.h"

void UNS_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNS_MainMenu::PlayAnimationShowR()
{
	PlayAnimation(ShowR);
}

void UNS_MainMenu::PlayAnimationShowL()
{
	PlayAnimation(ShowL);
}
