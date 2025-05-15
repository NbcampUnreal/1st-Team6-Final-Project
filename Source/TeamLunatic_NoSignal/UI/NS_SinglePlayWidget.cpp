// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_SinglePlayWidget.h"
#include "Components/Button.h"

void UNS_SinglePlayWidget::NativeConstruct()
{
    if (BP_WidgetToggleButton_NewGame)
    {
        BP_WidgetToggleButton_NewGame->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnNewGameClicked);
    }

    if (BP_WidgetToggleButton_LoadGame)
    {
        BP_WidgetToggleButton_LoadGame->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnLoadGameClicked);
    }

    if (BP_WidgetToggleButton_MainMenu)
    {
        BP_WidgetToggleButton_MainMenu->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnMainMenuClicked);
    }
}

void UNS_SinglePlayWidget::HandleButtonClicked(EWidgetToggleType ToggleType)
{
  
}

void UNS_SinglePlayWidget::OnNewGameClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("OnNewGameClicked"));
}

void UNS_SinglePlayWidget::OnLoadGameClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("OnLoadGameClicked"));
}

void UNS_SinglePlayWidget::OnMainMenuClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("OnMainMenuClicked"));
}
