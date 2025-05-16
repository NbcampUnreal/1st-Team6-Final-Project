#include "UI/NS_StartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/TempGameInstance.h"
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
    
    //UTempGameInstance* GS = Cast< UTempGameInstance>(GetGameInstance());
    //if (GS)
    //{
    //    //UGameplayStatics::OpenLevel(this, FName("NoSignalEntry"));
    //    GS->GetUIManager()->GetNS_MainMenuWidget()->SelectWidget(EWidgetToggleType::SinglePlayer);
    //    UE_LOG(LogTemp, Warning, TEXT("OnSinglePlayerClicked"));
    //}
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("Error!! Empty GameInstance / OnSinglePlayerClicked"));
}

void UNS_StartMenu::OnSettingsClicked()
{
    HideWidget();//SetVisibility(ESlateVisibility::Hidden);
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::Settings))
        Widget->ShowWidget();

    //UTempGameInstance* GS = Cast< UTempGameInstance>(GetGameInstance());
    //if (GS)
    //{
    //    GS->GetUIManager()->GetNS_MainMenuWidget()->SelectWidget(EWidgetToggleType::Settings);
    //    UE_LOG(LogTemp, Warning, TEXT("OnSettingsClicked"));
    //}
    //else
    //     UE_LOG(LogTemp, Warning, TEXT("Error!! Empty GameInstance /OnSettingsClicked"));
}

void UNS_StartMenu::OnQuitClicked()
{
    HideWidget();//SetVisibility(ESlateVisibility::Hidden);
    MainMenu->SelectWidget(EWidgetToggleType::Quit);
   
    //UTempGameInstance* GS = Cast< UTempGameInstance>(GetGameInstance());
    //if (GS)
    //{
    //    GS->GetUIManager()->GetNS_MainMenuWidget()->SelectWidget(EWidgetToggleType::Quit);
    //    UE_LOG(LogTemp, Warning, TEXT("OnQuitClicked"));
    //}
    //else
    //   UE_LOG(LogTemp, Warning, TEXT("Error!! Empty GameInstance /OnQuitClicked"));
}

void UNS_StartMenu::OnMultiPlayerClicked()
{
    HideWidget();//SetVisibility(ESlateVisibility::Hidden);
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::MultiPlayer))
        Widget->ShowWidget();

    //UTempGameInstance* GS = Cast< UTempGameInstance>(GetGameInstance());
    //if (GS)
    //{
    //    GS->GetUIManager()->GetNS_MainMenuWidget()->SelectWidget(EWidgetToggleType::MultiPlayer);
    //    UE_LOG(LogTemp, Warning, TEXT("OnMultiPlayerClicked"));
    //}
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("Error!! Empty GameInstance /OnMultiPlayerClicked"));
}
