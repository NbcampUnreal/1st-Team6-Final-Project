#include "UI/NS_UIManager.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameStartMenu.h"
#include "UI/NS_MasterMenuPanel.h"

UNS_UIManager::UNS_UIManager()
{
    if (!InGameMenuWidgetClass)
    {
        static ConstructorHelpers::FClassFinder<UNS_BaseMainMenu> BP_InGameMenu_ksw(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_InGameMenu_ksw.BP_InGameMenu_ksw_C"));
        if (BP_InGameMenu_ksw.Succeeded())
            InGameMenuWidgetClass = BP_InGameMenu_ksw.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("InGameMenuWidgetClass: %s"), *GetNameSafe(InGameMenuWidgetClass));
    }
}

void UNS_UIManager::InitUi(UWorld* World)
{
    //if (InGameMenuWidgetClass)
    //    InGameMenuWidget = CreateWidget<UNS_BaseMainMenu>(World, InGameMenuWidgetClass);
}
bool UNS_UIManager::IsInViewportInGameMenuWidget()
{
    if (InGameMenuWidget && InGameMenuWidget->GetVisibility() == ESlateVisibility::Visible )//InGameMenuWidget->IsInViewport()
        return true;
    return false;
}
void UNS_UIManager::ShowInGameMenuWidget(UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!InGameMenuWidget && InGameMenuWidgetClass && World)
    {
        InGameMenuWidget = CreateWidget<UNS_BaseMainMenu>(PC, InGameMenuWidgetClass);
    }
   
    if (InGameMenuWidget)
    {
        if (!InGameMenuWidget->IsInViewport())
            InGameMenuWidget->AddToViewport();

        InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);
        InGameMenuWidget->GetWidget(EWidgetToggleType::InGamemStartMenu)->ShowWidgetD();
    }
    //FInputModeUIOnly InputMode;
    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(InGameMenuWidget->TakeWidget()); // UI 위젯을 포커싱
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 락 해제
    InputMode.SetHideCursorDuringCapture(false); // 마우스 커서 보이게

    PC = World->GetFirstPlayerController();
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
}
void UNS_UIManager::HideInGameMenuWidget(UWorld* World)
{
    if (InGameMenuWidget && InGameMenuWidget->IsInViewport())
    {
        InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);

        APlayerController* PC = World->GetFirstPlayerController();
        PC->bShowMouseCursor = false;
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }
}