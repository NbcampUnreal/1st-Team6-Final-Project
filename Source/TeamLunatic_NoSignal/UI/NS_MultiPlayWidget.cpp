// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_MultiPlayWidget.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/TempGameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_BaseMainMenu.h"
#include "Kismet/GameplayStatics.h"

void UNS_MultiPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BP_WidgetToggleButton_HostServer)
        BP_WidgetToggleButton_HostServer->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_MultiPlayWidget::OnHostServerClicked);

    if (BP_WidgetToggleButton_ServerBrowser)
        BP_WidgetToggleButton_ServerBrowser->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_MultiPlayWidget::OnServerBrowserClicked);

    if (BP_WidgetToggleButton_MainMenu)
        BP_WidgetToggleButton_MainMenu->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_MultiPlayWidget::OnMainMenuClicked);
}

void UNS_MultiPlayWidget::Init(UNS_BaseMainMenu* NsMainMenu)
{
    Super::Init(NsMainMenu);
    SubMenus.Add(EWidgetToggleType::HostServer, MainMenu->GetWidget(EWidgetToggleType::HostServer));
    SubMenus.Add(EWidgetToggleType::ServerBrowser, MainMenu->GetWidget(EWidgetToggleType::ServerBrowser));
    SubMenus.Add(EWidgetToggleType::HostNewGame, MainMenu->GetWidget(EWidgetToggleType::HostNewGame));
    SubMenus.Add(EWidgetToggleType::HostLoadGame, MainMenu->GetWidget(EWidgetToggleType::HostLoadGame));
}

void UNS_MultiPlayWidget::OnHostServerClicked()
{
    HideSubMenuWidget();
    MainMenu->GetWidget(EWidgetToggleType::HostServer)->ShowWidget();
}

void UNS_MultiPlayWidget::OnServerBrowserClicked()
{
    HideSubMenuWidget();
    MainMenu->GetWidget(EWidgetToggleType::ServerBrowser)->ShowWidget();
}
void UNS_MultiPlayWidget::OnMainMenuClicked()
{
    HideSubMenuWidget();
    HideWidget();
    MainMenu->GetWidget(EWidgetToggleType::MainMenu)->ShowWidget();
}