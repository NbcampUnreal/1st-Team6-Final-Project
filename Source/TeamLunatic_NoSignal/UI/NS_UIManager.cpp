#include "UI/NS_UIManager.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameMenu.h"
#include "UI/NS_InGameStartMenu.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_Msg_GameOver.h"
#include "UI/NS_InGameMsg.h"
#include "UI/NS_PlayerHUD.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"

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
    if (!NS_MsgGameOverWidgetClass)
    {

        static ConstructorHelpers::FClassFinder<UNS_Msg_GameOver> BP_GameOveWidget(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/BP_NS_Msg_GameOver.BP_NS_Msg_GameOver_C"));
        if (BP_GameOveWidget.Succeeded())
            NS_MsgGameOverWidgetClass = BP_GameOveWidget.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("NS_MsgGameOverWidgetClass: %s"), *GetNameSafe(NS_MsgGameOverWidgetClass));
    }
    if (!NS_InGameMsgWidgetClass)
    {
        static ConstructorHelpers::FClassFinder<UNS_InGameMsg> WBP_InGameMessage(TEXT("/Game/UI/Blueprints/WBP_InGameMessage.WBP_InGameMessage_C"));
        if (WBP_InGameMessage.Succeeded())
            NS_InGameMsgWidgetClass = WBP_InGameMessage.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("NS_InGameMsgWidgetClass: %s"), *GetNameSafe(NS_InGameMsgWidgetClass));
    }

    if (!NS_PlayerHUDWidgetClass)
    {
        static ConstructorHelpers::FClassFinder<UNS_PlayerHUD> WBP_PlayerHUD(TEXT("/Game/SurvivalGameKitV2/Blueprints/Widgets/WBP_PlayerHUD.WBP_PlayerHUD_C"));
        if (WBP_PlayerHUD.Succeeded())
            NS_PlayerHUDWidgetClass = WBP_PlayerHUD.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("NS_PlayerHUDWidgetClass: %s"), *GetNameSafe(NS_PlayerHUDWidgetClass));
    }
}

void UNS_UIManager::InitUi(UWorld* World)
{
    //if (InGameMenuWidgetClass)
    //    InGameMenuWidget = CreateWidget<UNS_BaseMainMenu>(World, InGameMenuWidgetClass);
}
UNS_QuickSlotPanel* UNS_UIManager::GetQuickSlotPanel()
{
    if (NS_PlayerHUDWidget)
        return NS_PlayerHUDWidget->NS_QuickSlotPanel;
    return nullptr;
}
bool UNS_UIManager::ShowPlayerHUDWidget( UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!NS_PlayerHUDWidget || NS_PlayerHUDWidget && !NS_PlayerHUDWidget->IsInViewport())
    {
        if (NS_PlayerHUDWidgetClass)
            NS_PlayerHUDWidget = CreateWidget<UNS_PlayerHUD>(PC, NS_PlayerHUDWidgetClass);
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY NS_PlayerHUDWidgetClass!!!!!"));
            return false;
        }
        NS_PlayerHUDWidget->AddToViewport();
    }
    if (NS_PlayerHUDWidget)
    {
        NS_PlayerHUDWidget->ShowWidget();
        return true;
    }
    return false;
}
void UNS_UIManager::HidePlayerHUDWidget(UWorld* World)
{
    if (NS_PlayerHUDWidget && NS_PlayerHUDWidget->IsInViewport())
        NS_PlayerHUDWidget->HideWidget();
}
bool UNS_UIManager::ShowGameMsgWidget(FString& GameMsg, UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!NS_InGameMsgWidget || NS_InGameMsgWidget && !NS_InGameMsgWidget->IsInViewport())
    {
        if (NS_InGameMsgWidgetClass)
            NS_InGameMsgWidget = CreateWidget<UNS_InGameMsg>(PC, NS_InGameMsgWidgetClass);
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY NS_InGameMsgWidgetClass!!!!!"));
            return false;
        }
        NS_InGameMsgWidget->AddToViewport();
    }
    if (NS_InGameMsgWidget)
    {
        NS_InGameMsgWidget->ShowMessageText(GameMsg);
        return true;
    }
    return false;
}
void UNS_UIManager::HideGameMsgWidget(UWorld* World)
{
    if (NS_InGameMsgWidget && NS_InGameMsgWidget->IsInViewport())
        NS_InGameMsgWidget->HideWidget();
}
bool UNS_UIManager::ShowGameOverWidget(UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!NS_Msg_GameOveWidget || NS_Msg_GameOveWidget && !NS_Msg_GameOveWidget->IsInViewport())//!IsValid(NS_Msg_GameOveWidget) )
    {
        if (NS_MsgGameOverWidgetClass)
            NS_Msg_GameOveWidget = CreateWidget<UNS_Msg_GameOver>(PC, NS_MsgGameOverWidgetClass);
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY NS_MsgGameOverWidgetClass!!!!!"));
            return false;
        }
        NS_Msg_GameOveWidget->AddToViewport();
    }

    if (NS_Msg_GameOveWidget)
    {
        NS_Msg_GameOveWidget->ShowWidgetD();
        SetFInputModeGameAndUI(PC, NS_Msg_GameOveWidget);
        return true;
    }
    return false;
}
void UNS_UIManager::HideGameOverWidget(UWorld* World)
{
    if (NS_Msg_GameOveWidget && NS_Msg_GameOveWidget->IsInViewport())
    {
        //NS_Msg_GameOveWidget->RemoveFromParent();
        //NS_Msg_GameOveWidget = nullptr;
        NS_Msg_GameOveWidget->HideWidget();
        APlayerController* PC = World->GetFirstPlayerController();
        SetFInputModeGameOnly(PC);
    }
}
void UNS_UIManager::SetFInputModeGameAndUI(APlayerController* PC,UUserWidget* Widget)
{
    //FInputModeUIOnly InputMode;
    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(Widget->TakeWidget()); // UI 위젯을 포커싱
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 락 해제
    InputMode.SetHideCursorDuringCapture(false); // 마우스 커서 보이게

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
}
void UNS_UIManager::SetFInputModeGameOnly(APlayerController* PC)
{
    PC->bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
}
bool UNS_UIManager::IsInViewportInGameMenuWidget()
{
    if (InGameMenuWidget && InGameMenuWidget->GetVisibility() == ESlateVisibility::Visible )//InGameMenuWidget->IsInViewport()
        return true;
    return false;
}
bool UNS_UIManager::ShowInGameMenuWidget(UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!InGameMenuWidget || InGameMenuWidget && !InGameMenuWidget->IsInViewport())//!IsValid(InGameMenuWidget))
    {
        if (InGameMenuWidgetClass)
            InGameMenuWidget = CreateWidget<UNS_InGameMenu>(PC, InGameMenuWidgetClass);
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY InGameMenuWidgetClass!!!!!"));
            return false;
        }
        InGameMenuWidget->AddToViewport();
    }
    if (InGameMenuWidget)
    {
        InGameMenuWidget->ShowWidget();

        SetFInputModeGameAndUI(PC, InGameMenuWidget);

        return true;
    }
    return false;
}
void UNS_UIManager::ShowLoadGameWidget(UWorld* World)
{
    InGameMenuWidget->GetWidget(EWidgetToggleType::LoadMenuInGameOver)->ShowWidgetD();
    APlayerController* PC = World->GetFirstPlayerController();
    SetFInputModeGameAndUI(PC, InGameMenuWidget);
}
void UNS_UIManager::HideInGameMenuWidget(UWorld* World)
{
    if (InGameMenuWidget && InGameMenuWidget->IsInViewport())
    {
        InGameMenuWidget->HideWidget();

        APlayerController* PC = World->GetFirstPlayerController();
        SetFInputModeGameOnly(PC);
    }
}
