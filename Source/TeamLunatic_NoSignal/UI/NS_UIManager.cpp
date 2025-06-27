// NS_UIManager.cpp - 게임의 모든 UI 위젯을 관리하는 중앙 매니저 클래스 구현

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
    // 생성자에서 위젯 클래스 참조 설정
    
    // 인게임 메뉴 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_InGameMenu> WBP_InGameMenu(TEXT("/Game/UI/Blueprints/WBP_InGameMenu.WBP_InGameMenu_C"));
    if (WBP_InGameMenu.Succeeded())
        InGameMenuWidgetClass = WBP_InGameMenu.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("InGameMenuWidgetClass: %s"), *GetNameSafe(InGameMenuWidgetClass));

    // 게임 오버 메시지 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_Msg_GameOver> WBP_GameOver(TEXT("/Game/UI/Blueprints/WBP_GameOver.WBP_GameOver_C"));
    if (WBP_GameOver.Succeeded())
        NS_MsgGameOverWidgetClass = WBP_GameOver.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("NS_MsgGameOverWidgetClass: %s"), *GetNameSafe(NS_MsgGameOverWidgetClass));

    // 인게임 메시지 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_InGameMsg> WBP_InGameMsg(TEXT("/Game/UI/Blueprints/WBP_InGameMsg.WBP_InGameMsg_C"));
    if (WBP_InGameMsg.Succeeded())
        NS_InGameMsgWidgetClass = WBP_InGameMsg.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("NS_InGameMsgWidgetClass: %s"), *GetNameSafe(NS_InGameMsgWidgetClass));

    // 플레이어 HUD 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_PlayerHUD> WBP_PlayerHUD(TEXT("/Game/UI/Blueprints/WBP_PlayerHUD.WBP_PlayerHUD_C"));
    if (WBP_PlayerHUD.Succeeded())
        NS_PlayerHUDWidgetClass = WBP_PlayerHUD.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("NS_PlayerHUDWidgetClass: %s"), *GetNameSafe(NS_PlayerHUDWidgetClass));

    // 히트 이펙트 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HitEffect(TEXT("/Game/UI/Blueprints/WBP_HitEffect.WBP_HitEffect_C"));
    if (WBP_HitEffect.Succeeded())
        HitEffectWidgetClass = WBP_HitEffect.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("HitEffectWidgetClass: %s"), *GetNameSafe(HitEffectWidgetClass));

    // 관전자 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_SpectatorWidgetClass> WBP_Spectator(TEXT("/Game/UI/Blueprints/WBP_Spectator.WBP_Spectator_C"));
    if (WBP_Spectator.Succeeded())
        SpectatorWidgetClass = WBP_Spectator.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("SpectatorWidgetClass: %s"), *GetNameSafe(SpectatorWidgetClass));

    // 로딩 화면 위젯 클래스 찾기
    static ConstructorHelpers::FClassFinder<UNS_LoadingScreen> WBP_LoadingScreen(TEXT("/Game/UI/Blueprints/WBP_LoadingScreen.WBP_LoadingScreen_C"));
    if (WBP_LoadingScreen.Succeeded())
        NS_LoadingScreenClass = WBP_LoadingScreen.Class;
    else
        UE_LOG(LogTemp, Warning, TEXT("NS_LoadingScreenClass: %s"), *GetNameSafe(NS_LoadingScreenClass));
}

void UNS_UIManager::InitUi(UWorld* World)
{
    // UI 시스템 초기화 함수
    // 현재는 비어있지만, 필요한 경우 여기에 초기화 코드 추가
}

UNS_QuickSlotPanel* UNS_UIManager::GetQuickSlotPanel()
{
    // 플레이어 HUD에서 퀵슬롯 패널 가져오기
    if (NS_PlayerHUDWidget)
        return NS_PlayerHUDWidget->NS_QuickSlotPanel;
    return nullptr;
}

void UNS_UIManager::HidePlayerHUDWidget(UWorld* World)
{
    // 플레이어 HUD 위젯이 존재하고 뷰포트에 있으면 숨기기
    if (NS_PlayerHUDWidget && NS_PlayerHUDWidget->IsInViewport())
        NS_PlayerHUDWidget->HideWidget();
}

bool UNS_UIManager::ShowGameMsgWidget(FString& GameMsg, UWorld* World)
{
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    
    // 인게임 메시지 위젯이 없거나 뷰포트에 없으면 생성
    if (!NS_InGameMsgWidget || NS_InGameMsgWidget && !NS_InGameMsgWidget->IsInViewport())
    {
        if (NS_InGameMsgWidgetClass)
            NS_InGameMsgWidget = CreateWidget<UNS_InGameMsg>(PC, NS_InGameMsgWidgetClass);
        else
        {
            // 위젯 클래스가 없으면 오류 로그 출력 후 실패 반환
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY NS_InGameMsgWidgetClass!!!!!"));
            return false;
        }
        NS_InGameMsgWidget->AddToViewport();
    }
    
    // 위젯이 존재하면 메시지 표시 후 성공 반환
    if (NS_InGameMsgWidget)
    {
        NS_InGameMsgWidget->ShowMessageText(GameMsg);
        return true;
    }
    return false;
}

void UNS_UIManager::HideGameMsgWidget(UWorld* World)
{
    // 인게임 메시지 위젯이 존재하고 뷰포트에 있으면 숨기기
    if (NS_InGameMsgWidget && NS_InGameMsgWidget->IsInViewport())
        NS_InGameMsgWidget->HideWidget();
}

bool UNS_UIManager::ShowGameOverWidget(UWorld* World)
{
    // 서버에서는 실행하지 않음
    if (!World || World->IsNetMode(NM_DedicatedServer))
    {
        return false;
    }

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        return false;
    }

    // 게임 오버 위젯이 없거나 뷰포트에 없으면 생성
    if (!NS_Msg_GameOveWidget || !NS_Msg_GameOveWidget->IsInViewport())
    {
        if (NS_MsgGameOverWidgetClass)
        {
            NS_Msg_GameOveWidget = CreateWidget<UNS_Msg_GameOver>(PC, NS_MsgGameOverWidgetClass);
            NS_Msg_GameOveWidget->AddToViewport();
        }
        else
        {
            return false;
        }
    }

    // 위젯이 존재하면 표시하고 입력 모드 설정 후 성공 반환
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
    // 게임 오버 위젯이 존재하고 뷰포트에 있으면 숨기기
    if (NS_Msg_GameOveWidget && NS_Msg_GameOveWidget->IsInViewport())
    {
        NS_Msg_GameOveWidget->HideWidget();
        APlayerController* PC = World->GetFirstPlayerController();
        SetFInputModeGameOnly(PC);
    }
}
void UNS_UIManager::SetFInputModeGameAndUI(APlayerController* PC,UUserWidget* Widget)
{
    // 게임 및 UI 입력 모드 설정
    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(Widget->TakeWidget()); // UI 위젯을 포커싱
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 락 해제
    InputMode.SetHideCursorDuringCapture(false); // 마우스 커서 보이게

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true; // 마우스 커서 표시
}
void UNS_UIManager::SetFInputModeGameOnly(APlayerController* PC)
{
    // 게임 전용 입력 모드 설정
    PC->bShowMouseCursor = false; // 마우스 커서 숨김
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
}

void UNS_UIManager::CloseLoadingUI()
{
    // 로딩 화면이 존재하고 뷰포트에 있으면 제거
    if (NS_LoadingScreen && NS_LoadingScreen->IsInViewport())
    {
        UE_LOG(LogTemp, Log, TEXT("Remove Loading Screen!!!!"));
        NS_LoadingScreen->RemoveFromParent();
        NS_LoadingScreen = nullptr;
    }
}
void UNS_UIManager::CompleteLoadingProcess()
{
    // 로딩 화면이 존재하면 로딩 완료 처리
    if (NS_LoadingScreen)
    {
        NS_LoadingScreen->LevelLoadComplete();
    }
}

void UNS_UIManager::LoadingScreen(UWorld* World)
{
    // 로딩 화면 초기화
    NS_LoadingScreen = nullptr;

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    
    // 로딩 화면 생성 및 표시
    NS_LoadingScreen = CreateWidget<UNS_LoadingScreen>(PC, NS_LoadingScreenClass);

    // 로딩 완료 델리게이트 초기화
    OnLoadingFinished.Unbind();
    
    // 로딩 화면을 뷰포트에 추가하고 진행 상태 업데이트
    NS_LoadingScreen->AddToViewport();
    NS_LoadingScreen->UpdateProgress();
}

bool UNS_UIManager::IsInViewportInGameMenuWidget()
{
    // 인게임 메뉴 위젯이 존재하고 보이는 상태인지 확인
    if (InGameMenuWidget && InGameMenuWidget->GetVisibility() == ESlateVisibility::Visible)
        return true;
    return false;
}
bool UNS_UIManager::ShowInGameMenuWidget(UWorld* World)
{
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    
    // 인게임 메뉴 위젯이 없거나 뷰포트에 없으면 생성
    if (!InGameMenuWidget || InGameMenuWidget && !InGameMenuWidget->IsInViewport())
    {
        if (InGameMenuWidgetClass)
            InGameMenuWidget = CreateWidget<UNS_InGameMenu>(PC, InGameMenuWidgetClass);
        else
        {
            // 위젯 클래스가 없으면 오류 로그 출력 후 실패 반환
            UE_LOG(LogTemp, Warning, TEXT("ERROR!!! EMPTY InGameMenuWidgetClass!!!!!"));
            return false;
        }
        InGameMenuWidget->AddToViewport();
    }
    
    // 위젯이 존재하면 표시하고 입력 모드 설정 후 성공 반환
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
    // 인게임 메뉴의 로드 게임 위젯 표시
    InGameMenuWidget->GetWidget(EWidgetToggleType::LoadMenuInGameOver)->ShowWidgetD();
    
    // 플레이어 컨트롤러 가져와서 입력 모드 설정
    APlayerController* PC = World->GetFirstPlayerController();
    SetFInputModeGameAndUI(PC, InGameMenuWidget);
}
void UNS_UIManager::HideInGameMenuWidget(UWorld* World)
{
    // 인게임 메뉴 위젯이 존재하고 뷰포트에 있으면 숨기기
    if (InGameMenuWidget && InGameMenuWidget->IsInViewport())
    {
        InGameMenuWidget->HideWidget();

        // 플레이어 컨트롤러 가져와서 게임 전용 입력 모드로 설정
        APlayerController* PC = World->GetFirstPlayerController();
        SetFInputModeGameOnly(PC);
    }
}

bool UNS_UIManager::ShowSpectatorWidget(UWorld* World)
{
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return false;

    // 관전자 위젯이 없거나 뷰포트에 없으면 생성
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
            // 위젯 클래스가 없으면 오류 로그 출력 후 실패 반환
            UE_LOG(LogTemp, Error, TEXT("SpectatorWidgetClass is NULL!"));
            return false;
        }
    }

    // 입력 모드 설정 후 성공 반환
    SetFInputModeGameAndUI(PC, SpectatorWidget);
    return true;
}


void UNS_UIManager::ShowHitEffectWidget(UWorld* World)
{
    // 월드 유효성 검사
    if (!World) return;

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC || !PC->IsLocalController()) return;

    // 히트 이펙트 위젯 클래스가 있으면 위젯 생성 및 표시
    if (HitEffectWidgetClass)
    {
        UUserWidget* HitWidget = CreateWidget<UUserWidget>(PC, HitEffectWidgetClass);
        if (HitWidget)
        {
            HitWidget->AddToViewport();

            // 0.5초 후 위젯 제거하는 타이머 설정
            FTimerHandle TempHandle;
            PC->GetWorldTimerManager().SetTimer(TempHandle, [HitWidget]()
            {
                if (HitWidget && HitWidget->IsInViewport())
                    HitWidget->RemoveFromParent();
            }, 0.5f, false);
        }
    }
}

// PlayerHUD 등록 함수
void UNS_UIManager::SetPlayerHUDWidget(UNS_PlayerHUD* InHUD)
{
    // 이미 등록되어 있다면 아무것도 하지 않음
    if (NS_PlayerHUDWidget) return;

    if (InHUD)
    {
        // HUD 등록 및 이벤트 방송
        NS_PlayerHUDWidget = InHUD;
        OnPlayerHUDReady.Broadcast(NS_PlayerHUDWidget);
        UE_LOG(LogTemp, Log, TEXT("PlayerHUD가 UIManager에 등록되고, OnPlayerHUDReady 이벤트가 방송되었습니다."));
    }
}

// 플레이어 HUD 위젯 표시 함수
bool UNS_UIManager::ShowPlayerHUDWidget(UWorld* World)
{
    // HUD가 존재하면 표시하고 성공 반환
    if (NS_PlayerHUDWidget)
    {
        NS_PlayerHUDWidget->ShowWidget();
        return true;
    }
    return false;
}