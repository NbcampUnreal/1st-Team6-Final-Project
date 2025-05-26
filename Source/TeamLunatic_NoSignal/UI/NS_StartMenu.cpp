#include "UI/NS_StartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"

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
}
