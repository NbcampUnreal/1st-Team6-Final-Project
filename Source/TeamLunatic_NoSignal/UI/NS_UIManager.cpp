// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_UIManager.h"
#include "UI/NS_MainMenu.h"

UNS_UIManager::UNS_UIManager()
{
    if (!MainMenuWidgetClass)
    {
        static ConstructorHelpers::FClassFinder<UNS_MainMenu> WBP_MainMenuWidget(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_MainMenu_ksw"));
        if (WBP_MainMenuWidget.Succeeded())
            MainMenuWidgetClass = WBP_MainMenuWidget.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass: %s"), *GetNameSafe(MainMenuWidgetClass));
    }
}

void UNS_UIManager::InitUi(UWorld* World)
{

}

void UNS_UIManager::ShowMainMenuWidget(UWorld* World)
{
    if (!MainMenuWidget && MainMenuWidgetClass && World)
        MainMenuWidget = CreateWidget<UNS_MainMenu>(World, MainMenuWidgetClass);

    if (MainMenuWidget && !MainMenuWidget->IsInViewport())
        MainMenuWidget->AddToViewport();
}
