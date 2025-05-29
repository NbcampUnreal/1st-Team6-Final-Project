// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_InGameStartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"              // UGameplayStatics::OpenLevel(), 
#include "OnlineSubsystem.h"                     // IOnlineSubsystem
#include "Interfaces/OnlineSessionInterface.h"   // IOnlineSessionPtr, DestroySession()
#include "Kismet/KismetSystemLibrary.h"          // KismetSystemLibrary::QuitGame()
#include "Blueprint/WidgetBlueprintLibrary.h"    // FInputModeGameOnly 등
#include "GameFlow/NS_GameInstance.h"

void UNS_InGameStartMenu::NativeConstruct()
{
	Super::NativeConstruct();
	if (BP_Resume)
        BP_Resume->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnResumeClicked);
    if (BP_SaveGame)
        BP_SaveGame->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnSaveGameClicked);
    if (BP_LoadGame)
        BP_LoadGame->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnLoadGameClicked);
    if (BP_Settings)
        BP_Settings->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnSettingsClicked);
    if (BP_MainMenu)
        BP_MainMenu->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::On_MainMenuClicked);
    //if (BP_Disconnect)
    //    BP_Disconnect->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnDisconnectClicked);
    if (BP_Quit)
        BP_Quit->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnQuitClicked);

   UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance());
  if (NS_GameInstance)
  {
      if (NS_GameInstance->bIsSinglePlayer)
      {
          BP_SaveGame->SetVisibility(ESlateVisibility::Visible);
          BP_LoadGame->SetVisibility(ESlateVisibility::Visible);
      }
      else
      {
          BP_SaveGame->SetVisibility(ESlateVisibility::Collapsed);
          BP_LoadGame->SetVisibility(ESlateVisibility::Collapsed);
      }
  }
}
void UNS_InGameStartMenu::Init(UNS_BaseMainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);
    SubMenus.Add(EWidgetToggleType::SaveGame, MainMenu->GetWidget(EWidgetToggleType::SaveGame));
    SubMenus.Add(EWidgetToggleType::LoadGame, MainMenu->GetWidget(EWidgetToggleType::LoadGame));
    SubMenus.Add(EWidgetToggleType::Settings, MainMenu->GetWidget(EWidgetToggleType::Settings));
}

void UNS_InGameStartMenu::ShowWidgetD()
{
    Super::ShowWidgetD();
    HideSubMenuWidget();
}

void UNS_InGameStartMenu::OnResumeClicked()
{
    if (MainMenu)
    {
        MainMenu->RemoveFromParent();
        MainMenu = nullptr;
    }

 /*   bool bIsCurrentVisible = GetVisibility() == ESlateVisibility::Visible;
    if (bIsCurrentVisible)
        HideWidget();
    else
        ShowWidgetD();*/
}

void UNS_InGameStartMenu::OnSaveGameClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::SaveGame))
        Widget->ShowWidgetD();
}

void UNS_InGameStartMenu::OnLoadGameClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::LoadGame))
        Widget->ShowWidgetD();
}

void UNS_InGameStartMenu::OnSettingsClicked()
{
    HideSubMenuWidget();
    HideWidget();
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::Settings))
        Widget->ShowWidgetD();
}

void UNS_InGameStartMenu::On_MainMenuClicked()
{
    //HideSubMenuWidget();
    HideWidget();
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->DestroySession(NAME_GameSession);
        }
    }
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("MainTitle")));//MenuMap / MainTitle
}

void UNS_InGameStartMenu::OnDisconnectClicked()
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->DestroySession(NAME_GameSession);
        }
    }
}

void UNS_InGameStartMenu::OnQuitClicked()
{
    OnDisconnectClicked();
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}
