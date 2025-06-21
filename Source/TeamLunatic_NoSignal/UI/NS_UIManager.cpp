#include "UI/NS_UIManager.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_InGameMenu.h"
#include "UI/NS_InGameStartMenu.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_Msg_GameOver.h"
#include "UI/NS_InGameMsg.h"
#include "UI/NS_PlayerHUD.h"
#include "UI/NS_LoadingScreen.h"
#include "UI/NS_SpectatorWidgetClass.h" 
#include "GameFlow/NS_GameInstance.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Containers/Ticker.h" 
#include "Kismet/GameplayStatics.h"

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
    if (!NS_LoadingScreenClass)
    {
        static ConstructorHelpers::FClassFinder<UNS_LoadingScreen> WBP_LoadingScreen(TEXT("/Game/UI/Blueprints/WBP_LoadingScreen.WBP_LoadingScreen_C"));
        if (WBP_LoadingScreen.Succeeded())
            NS_LoadingScreenClass = WBP_LoadingScreen.Class;
        else
            UE_LOG(LogTemp, Warning, TEXT("NS_LoadingScreenClass: %s"), *GetNameSafe(NS_LoadingScreenClass));
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
    if (!World || World->IsNetMode(NM_DedicatedServer)) 
    {
        return false;
    }

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
    UE_LOG(LogTemp, Warning, TEXT("ShowGameOverWidget: 함수 실행 시작."));
    UE_LOG(LogTemp, Warning, TEXT("ShowGameOverWidget: NetMode = %d"), static_cast<int32>(World->GetNetMode()));
    // 1. 전달받은 World가 유효한지, 데디케이티드 서버는 아닌지 확인합니다.
    if (!World || World->IsNetMode(NM_DedicatedServer))
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowGameOverWidget: 전달받은 월드가 유효하지 않거나 데디케이티드 서버이므로 UI를 표시하지 않습니다."));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("ShowGameOverWidget: 월드 유효성 검사 통과."));

    // 2. 전달받은 World에서 로컬 플레이어 컨트롤러를 가져옵니다.
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowGameOverWidget: World->GetFirstPlayerController()가 null을 반환했습니다."));
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("ShowGameOverWidget: 로컬 플레이어 컨트롤러 (%s) 가져오기 성공."), *PC->GetName());

    // 3. 위젯 생성 로직 (이하 동일)
    if (!NS_Msg_GameOveWidget || !NS_Msg_GameOveWidget->IsInViewport())
    {
        if (NS_MsgGameOverWidgetClass)
        {
            NS_Msg_GameOveWidget = CreateWidget<UNS_Msg_GameOver>(PC, NS_MsgGameOverWidgetClass);
            NS_Msg_GameOveWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("ShowGameOverWidget: 위젯 생성 및 뷰포트 추가 완료."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ShowGameOverWidget: NS_MsgGameOverWidgetClass가 설정되지 않았습니다!"));
            return false;
        }
    }

    // 4. 위젯 표시 및 입력 모드 설정 (이하 동일)
    if (NS_Msg_GameOveWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("ShowGameOverWidget: 위젯 표시 및 입력 모드 설정을 시작합니다."));
        NS_Msg_GameOveWidget->ShowWidgetD();
        SetFInputModeGameAndUI(PC, NS_Msg_GameOveWidget);
        UE_LOG(LogTemp, Warning, TEXT("ShowGameOverWidget: 모든 과정 성공, 게임 오버 UI가 표시됩니다."));
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

void UNS_UIManager::CloseLoadingUI()
{
    if (NS_LoadingScreen)
    {
        UE_LOG(LogTemp, Log, TEXT("Remove Loading Screen!!!!"));
		NS_LoadingScreen = nullptr;
        NS_LoadingScreen->RemoveFromParent();
        if (NS_LoadingScreen->IsInViewport()) NS_LoadingScreen->RemoveFromParent();
    }
}

void UNS_UIManager::LoadingScreen(UWorld* World)
{
    NS_LoadingScreen = nullptr;

    APlayerController* PC = World->GetFirstPlayerController();
   // if (!NS_LoadingScreen || NS_LoadingScreen && !NS_LoadingScreen->IsInViewport())
    NS_LoadingScreen = CreateWidget<UNS_LoadingScreen>(PC, NS_LoadingScreenClass);

    OnLoadingFinished.Unbind();
    NS_LoadingScreen->AddToViewport();
    NS_LoadingScreen->UpdateProgress();
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

bool UNS_UIManager::ShowSpectatorWidget(UWorld* World)
{
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return false;

    if (!SpectatorWidget || !SpectatorWidget->IsInViewport())
    {
        if (SpectatorWidgetClass)
        {
            SpectatorWidget = CreateWidget<UNS_SpectatorWidgetClass>(PC, SpectatorWidgetClass);
            if (SpectatorWidget)
            {
                SpectatorWidget->AddToViewport();
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SpectatorWidgetClass is NULL!"));
            return false;
        }
    }

    SetFInputModeGameAndUI(PC, SpectatorWidget);
    return true;
}
