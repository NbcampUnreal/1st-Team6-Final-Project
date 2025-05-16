#include "UI/NS_SinglePlayWidget.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/TempGameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"

void UNS_SinglePlayWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (BP_WidgetToggleButton_NewGame)
        BP_WidgetToggleButton_NewGame->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnNewGameClicked);

    if (BP_WidgetToggleButton_LoadGame)
        BP_WidgetToggleButton_LoadGame->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnLoadGameClicked);

    if (BP_WidgetToggleButton_MainMenu)
        BP_WidgetToggleButton_MainMenu->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_SinglePlayWidget::OnMainMenuClicked);
}

void UNS_SinglePlayWidget::Init(UNS_MainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);
  //  UE_LOG(LogTemp, Warning, TEXT("UNS_SinglePlayWidget NativeConstruct"));
    SubMenus.Add(EWidgetToggleType::NewGame, MainMenu->GetWidget(EWidgetToggleType::NewGame));
    SubMenus.Add(EWidgetToggleType::LoadGame, MainMenu->GetWidget(EWidgetToggleType::LoadGame));
}

void UNS_SinglePlayWidget::OnNewGameClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::NewGame))
        Widget->ShowWidget();
}

void UNS_SinglePlayWidget::OnLoadGameClicked()
{
    HideSubMenuWidget();
    if (UNS_MasterMenuPanel* Widget = SubMenus.FindRef(EWidgetToggleType::LoadGame))
        Widget->ShowWidget();
}

void UNS_SinglePlayWidget::OnMainMenuClicked()
{
    HideSubMenuWidget();
    HideWidget();
    MainMenu->GetWidget(EWidgetToggleType::MainMenu)->ShowWidget();
}
