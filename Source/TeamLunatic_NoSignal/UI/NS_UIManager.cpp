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
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

UNS_UIManager::UNS_UIManager()
{
    // 생성자에서 위젯 클래스 참조 설정
    
    // 인게임 메뉴 위젯 클래스 찾기 - 여러 경로 시도
    //static ConstructorHelpers::FClassFinder<UNS_InGameMenu> WBP_InGameMenu(TEXT("/Game/UI/Blueprints/WBP_InGameMenu.WBP_InGameMenu_C"));
    //if (WBP_InGameMenu.Succeeded())
    //{
    //    InGameMenuWidgetClass = WBP_InGameMenu.Class;
    //    UE_LOG(LogTemp, Warning, TEXT("InGameMenuWidgetClass 로드 성공: %s"), *GetNameSafe(InGameMenuWidgetClass));
    //}
    //else
    //{
    //    // 블루프린트를 찾지 못한 경우 C++ 클래스 직접 사용
    //    InGameMenuWidgetClass = UNS_InGameMenu::StaticClass();
    //    UE_LOG(LogTemp, Warning, TEXT("블루프린트를 찾지 못해 C++ 클래스 사용: %s"), *GetNameSafe(InGameMenuWidgetClass));
    //}

    //// 게임 오버 메시지 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UNS_Msg_GameOver> WBP_GameOver(TEXT("/Game/UI/Blueprints/WBP_GameOver.WBP_GameOver_C"));
    //if (WBP_GameOver.Succeeded())
    //    NS_MsgGameOverWidgetClass = WBP_GameOver.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("NS_MsgGameOverWidgetClass: %s"), *GetNameSafe(NS_MsgGameOverWidgetClass));

    //// 인게임 메시지 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UNS_InGameMsg> WBP_InGameMsg(TEXT("/Game/UI/Blueprints/WBP_InGameMsg.WBP_InGameMsg_C"));
    //if (WBP_InGameMsg.Succeeded())
    //    NS_InGameMsgWidgetClass = WBP_InGameMsg.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("NS_InGameMsgWidgetClass: %s"), *GetNameSafe(NS_InGameMsgWidgetClass));

    //// 플레이어 HUD 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UNS_PlayerHUD> WBP_PlayerHUD(TEXT("/Game/UI/Blueprints/WBP_PlayerHUD.WBP_PlayerHUD_C"));
    //if (WBP_PlayerHUD.Succeeded())
    //    NS_PlayerHUDWidgetClass = WBP_PlayerHUD.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("NS_PlayerHUDWidgetClass: %s"), *GetNameSafe(NS_PlayerHUDWidgetClass));

    //// 히트 이펙트 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HitEffect(TEXT("/Game/UI/Blueprints/WBP_HitEffect.WBP_HitEffect_C"));
    //if (WBP_HitEffect.Succeeded())
    //    HitEffectWidgetClass = WBP_HitEffect.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("HitEffectWidgetClass: %s"), *GetNameSafe(HitEffectWidgetClass));

    //// 관전자 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UNS_SpectatorWidgetClass> WBP_Spectator(TEXT("/Game/UI/Blueprints/WBP_Spectator.WBP_Spectator_C"));
    //if (WBP_Spectator.Succeeded())
    //    SpectatorWidgetClass = WBP_Spectator.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("SpectatorWidgetClass: %s"), *GetNameSafe(SpectatorWidgetClass));

    //// 로딩 화면 위젯 클래스 찾기
    //static ConstructorHelpers::FClassFinder<UNS_LoadingScreen> WBP_LoadingScreen(TEXT("/Game/UI/Blueprints/WBP_LoadingScreen.WBP_LoadingScreen_C"));
    //if (WBP_LoadingScreen.Succeeded())
    //    NS_LoadingScreenClass = WBP_LoadingScreen.Class;
    //else
    //    UE_LOG(LogTemp, Warning, TEXT("NS_LoadingScreenClass: %s"), *GetNameSafe(NS_LoadingScreenClass));
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
    if (NS_LoadingScreen && IsValid(NS_LoadingScreen))
    {
        UE_LOG(LogTemp, Log, TEXT("Remove Loading Screen!!!!"));
        NS_LoadingScreen->RemoveFromParent();
        NS_LoadingScreen = nullptr;
    }
}

void UNS_UIManager::HideLoadingScreen(UWorld* World)
{
    UE_LOG(LogTemp, Warning, TEXT("HideLoadingScreen 호출됨"));
    
    if (NS_LoadingScreen && IsValid(NS_LoadingScreen))
    {
        NS_LoadingScreen->RemoveFromParent();
        NS_LoadingScreen = nullptr;
        
        // 입력 모드를 게임 전용으로 변경
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            SetFInputModeGameOnly(PC);
        }
        
        UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 숨김 완료"));
    }
}
void UNS_UIManager::CompleteLoadingProcess()
{
    // 새로운 로딩 시스템에서는 자동으로 로딩 완료를 감지합니다
    UE_LOG(LogTemp, Warning, TEXT("CompleteLoadingProcess 호출됨 - 새로운 시스템에서는 자동 감지"));
}

void UNS_UIManager::LoadingScreen(UWorld* World)
{
    UE_LOG(LogTemp, Error, TEXT("=== LoadingScreen 함수 호출됨 (구버전) ==="));

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

    // 로딩 시작
    NS_LoadingScreen->StartLoading();

    UE_LOG(LogTemp, Error, TEXT("=== LoadingScreen 함수 완료 ==="));
}

void UNS_UIManager::ShowLoadingScreen(UWorld* World)
{
    UE_LOG(LogTemp, Error, TEXT("=== ShowLoadingScreen 호출됨 ==="));
    UE_LOG(LogTemp, Error, TEXT("현재 NS_LoadingScreen 상태: %s"), NS_LoadingScreen ? TEXT("존재함") : TEXT("null"));

    // 안전한 null 체크 및 유효성 검사
    if (NS_LoadingScreen && IsValid(NS_LoadingScreen))
    {
        // IsInViewport 안전 체크 (간단한 방법)
        bool bIsInViewport = false;

        // 강화된 유효성 체크 (UE5 호환)
        if (IsValid(NS_LoadingScreen))
        {
            // 매우 안전한 방법: 단계별 체크
            bool bCanCallIsInViewport = true;

            // 위젯의 파괴 상태 체크
            if (NS_LoadingScreen->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed))
            {
                bCanCallIsInViewport = false;
                UE_LOG(LogTemp, Error, TEXT("NS_LoadingScreen이 파괴 플래그 설정됨"));
            }

            if (bCanCallIsInViewport)
            {
                bIsInViewport = NS_LoadingScreen->IsInViewport();
            }
            else
            {
                NS_LoadingScreen = nullptr;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("NS_LoadingScreen이 유효하지 않음 - null로 설정"));
            NS_LoadingScreen = nullptr;
        }

        if (bIsInViewport)
        {
            UE_LOG(LogTemp, Error, TEXT("기존 로딩 스크린 재사용 - 연속 로딩 모드"));

            // 기존 로딩 스크린을 레벨 전환 모드로 전환
            if (!NS_LoadingScreen->IsLoadingComplete())
            {
                NS_LoadingScreen->StartLoading(); // 로딩 재시작
            }
            return;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("기존 로딩 스크린이 뷰포트에 없음 - 재생성"));
            NS_LoadingScreen = nullptr; // 안전하게 null로 설정
        }
    }
    else if (NS_LoadingScreen)
    {
        UE_LOG(LogTemp, Error, TEXT("NS_LoadingScreen이 유효하지 않음 - null로 설정"));
        NS_LoadingScreen = nullptr;
    }

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC || !NS_LoadingScreenClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowLoadingScreen: PlayerController 또는 LoadingScreenClass가 없습니다."));
        return;
    }

    // 로딩 화면 생성 및 표시
    NS_LoadingScreen = CreateWidget<UNS_LoadingScreen>(PC, NS_LoadingScreenClass);
    if (NS_LoadingScreen)
    {
        // 최상위 Z-Order로 추가하여 모든 것을 가림
        NS_LoadingScreen->AddToViewport(32767); // 최대 Z-Order

        // 강제로 보이게 설정
        NS_LoadingScreen->SetVisibility(ESlateVisibility::Visible);

        // 로딩 시작
        NS_LoadingScreen->StartLoading();

        // 입력 모드 설정 (에디터에서는 덜 제한적으로)
        if (GIsEditor)
        {
            // 에디터에서는 게임과 UI 모두 허용
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(NS_LoadingScreen->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            UE_LOG(LogTemp, Warning, TEXT("에디터 모드: GameAndUI 입력 모드 설정"));
        }
        else
        {
            // 패키징된 게임에서는 게임과 UI 모두 허용 (검은 화면 방지)
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(NS_LoadingScreen->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            UE_LOG(LogTemp, Error, TEXT("게임 모드: GameAndUI 입력 모드 설정 (검은 화면 방지)"));
        }

        UE_LOG(LogTemp, Warning, TEXT("새로운 로딩 화면 표시 완료 - Z-Order: 10000"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShowLoadingScreen: 로딩 화면 생성 실패"));
    }
}

// void UNS_UIManager::HideLoadingScreen(UWorld* World)
// {
//     if (NS_LoadingScreen && NS_LoadingScreen->IsInViewport())
//     {
//         // 로딩 스크린 제거
//         NS_LoadingScreen->RemoveFromParent();
//         NS_LoadingScreen = nullptr;
//
//         // 게임 입력 모드로 복원
//         if (APlayerController* PC = World->GetFirstPlayerController())
//         {
//             FInputModeGameOnly InputMode;
//             PC->SetInputMode(InputMode);
//             PC->bShowMouseCursor = false;
//
//             UE_LOG(LogTemp, Log, TEXT("로딩 화면 숨김 완료 - 게임 모드로 전환"));
//         }
//     }
// }

void UNS_UIManager::StartFrameRateCheck()
{
    if (!NS_LoadingScreen)
    {
        UE_LOG(LogTemp, Error, TEXT("StartFrameRateCheck: 로딩 스크린이 null입니다"));
        return;
    }

    if (!IsValid(NS_LoadingScreen))
    {
        UE_LOG(LogTemp, Error, TEXT("StartFrameRateCheck: 로딩 스크린이 유효하지 않습니다"));
        NS_LoadingScreen = nullptr;
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("StartFrameRateCheck: 기존 로딩 스크린을 연속 모드로 전환"));

    // 현재 진행률 확인
    float CurrentProgress = NS_LoadingScreen->GetCurrentProgress();
    UE_LOG(LogTemp, Warning, TEXT("연속 모드 전환 - 현재 진행률: %.1f%%"), CurrentProgress * 100.0f);

    // 로딩이 아직 진행 중이 아니라면 다시 시작
    if (!NS_LoadingScreen->IsLoadingComplete())
    {
        NS_LoadingScreen->StartLoading();
    }
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