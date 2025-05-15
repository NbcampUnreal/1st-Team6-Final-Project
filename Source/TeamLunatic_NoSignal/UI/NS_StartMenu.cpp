// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_StartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/TempGameInstance.h"
#include "UI/NS_UIManager.h"
#include "Kismet/GameplayStatics.h"

void UNS_StartMenu::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (BP_WidgetToggleButton_SinglePlayer)
    {
        BP_WidgetToggleButton_SinglePlayer->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnSinglePlayerClicked);
    }

    if (BP_WidgetToggleButton_Settings)
    {
        BP_WidgetToggleButton_Settings->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnSettingsClicked);
    }

    if (BP_WidgetToggleButton_Quit)
    {
        BP_WidgetToggleButton_Quit->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnQuitClicked);
    }

    if (BP_WidgetToggleButton_MultiPlayer)
    {
        BP_WidgetToggleButton_MultiPlayer->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnMultiPlayerClicked);
    }
    
}
void UNS_StartMenu::OnSinglePlayerClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UTempGameInstance* GS = Cast< UTempGameInstance>(GetGameInstance());
    if (GS)
    {
        UGameplayStatics::OpenLevel(this, FName("NoSignalEntry"));
      //  GS->GetUIManager()->GetNS_MainMenuWidget()->
        UE_LOG(LogTemp, Warning, TEXT("OnSinglePlayerClicked"));
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("Error!! Empty GameInstance / OnSinglePlayerClicked"));

}

void UNS_StartMenu::OnSettingsClicked()
{
    SetVisibility(ESlateVisibility::Hidden);

    UE_LOG(LogTemp, Warning, TEXT("OnSettingsClicked"));
}

void UNS_StartMenu::OnQuitClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("OnQuitClicked"));
}

void UNS_StartMenu::OnMultiPlayerClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("OnMultiPlayerClicked"));
}
