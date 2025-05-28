#include "UI/NS_StartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_InGameStartMenu.h"

void UNS_StartMenu::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (BP_WidgetToggleButton_SinglePlayer)
        BP_WidgetToggleButton_SinglePlayer->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnSinglePlayerClicked);

    if (BP_WidgetToggleButton_Settings)
        BP_WidgetToggleButton_Settings->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnSettingsClicked);

    if (BP_WidgetToggleButton_Quit)
        BP_WidgetToggleButton_Quit->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnQuitClicked);

    if (BP_WidgetToggleButton_MultiPlayer)
        BP_WidgetToggleButton_MultiPlayer->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_StartMenu::OnMultiPlayerClicked);
}
void UNS_StartMenu::OnSinglePlayerClicked()
{
    HideWidget();
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::SinglePlayer))
        Widget->ShowWidget();

    //if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu))
    //{
    //    if (UNS_InGameStartMenu* NS_InGameStartMenu = Cast<UNS_InGameStartMenu>(Widget))
    //    {
    //        NS_InGameStartMenu->bIsSinglePlayer = false;
    //    }
    //}
    UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance());
	if (NS_GameInstance)
	{
        NS_GameInstance->bIsSinglePlayer = true; // 싱글 플레이어 모드로 설정
		UE_LOG(LogTemp, Warning, TEXT("싱글 플레이어 모드로 설정됨"));
	}
	
}

void UNS_StartMenu::OnSettingsClicked()
{
    HideWidget();//SetVisibility(ESlateVisibility::Hidden);
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::Settings))
        Widget->ShowWidget();
}

void UNS_StartMenu::OnQuitClicked()
{
    HideWidget();
    MainMenu->QuitNSGame();
}

void UNS_StartMenu::OnMultiPlayerClicked()
{
    HideWidget();
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::MultiPlayer))
        Widget->ShowWidget();

    //if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::InGamemStartMenu))
    //{
    //   if (UNS_InGameStartMenu* NS_InGameStartMenu = Cast<UNS_InGameStartMenu>(Widget))
    //   {
    //       NS_InGameStartMenu->bIsSinglePlayer = false;
    //   }
    //}
    UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance());
    if (NS_GameInstance)
    {
        NS_GameInstance->bIsSinglePlayer = false; // 싱글 플레이어 모드로 설정
        UE_LOG(LogTemp, Warning, TEXT("싱글 플레이어 모드로 설정됨"));
    }
}
