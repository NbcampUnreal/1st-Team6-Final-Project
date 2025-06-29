#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_Msg_GameOver.h" 
#include "UI/NS_PlayerHUD.h"
#include "Blueprint/UserWidget.h" 
#include "Inventory UI/NS_InventoryHUD.h"

/**
 * 플레이어 컨트롤러 생성자
 * 기본 설정을 초기화합니다.
 */
ANS_PlayerController::ANS_PlayerController()
{
    // 마우스 커서 초기 설정 (기본적으로 숨김, 인게임 메뉴에서만 표시)
    bShowMouseCursor = false;
    
    // 클릭 이벤트 활성화 (UI 요소와 상호작용 가능)
    bEnableClickEvents = true;
    
    // 마우스 오버 이벤트 활성화 (UI 요소에 마우스를 올렸을 때 반응)
    bEnableMouseOverEvents = true;
}

/**
 * 게임 시작 시 호출되는 함수
 * 플레이어 HUD를 초기화하고 UI 매니저에 등록합니다.
 */
void ANS_PlayerController::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();

    // 로컬 플레이어 컨트롤러인 경우에만 HUD 초기화
    if (IsLocalPlayerController())
    {
        // PlayerHUDClass가 설정되어 있는지 확인
        if (PlayerHUDClass)
        {
            // PlayerHUD 위젯 생성
            PlayerHUDWidget = CreateWidget<UNS_PlayerHUD>(this, PlayerHUDClass);

            if (PlayerHUDWidget)
            {
                // 뷰포트에 HUD 추가
                PlayerHUDWidget->AddToViewport();
                
                // HUD 표시
                PlayerHUDWidget->ShowWidget();

                // 게임 인스턴스를 통해 UI 매니저 접근
                if (UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>())
                {
                    if (UNS_UIManager* UIManager = GI->GetUIManager())
                    {
                        // UI 매니저에 플레이어 HUD 등록
                        UIManager->SetPlayerHUDWidget(PlayerHUDWidget);
                    }
                }
            }
        }
    }
}

/**
 * 입력 컴포넌트 설정 함수
 * 키 바인딩을 설정합니다.
 */
void ANS_PlayerController::SetupInputComponent()
{
    // 부모 클래스의 SetupInputComponent 호출
    Super::SetupInputComponent();

    // InputComponent가 유효한지 확인
    if (InputComponent)
    {
        // 인게임 메뉴 토글 액션 바인딩 (M 또는 ESC 키)
        InputComponent->BindAction("ToggleInGameMenu", IE_Pressed, this, &ANS_PlayerController::ToggleInGameMenu);
    }
}

/**
 * 인게임 메뉴 토글 함수
 * J 키(에디터) 또는 ESC 키(패키징 버전)를 눌러 인게임 메뉴를 표시하거나 숨깁니다.
 * ProjectSetting>Input>입력매핑>추가 "ToggleInGameMenu" (단축키"J"설정)
 */
void ANS_PlayerController::ToggleInGameMenu()
{
#if WITH_EDITOR
    // 에디터에서는 J 키를 사용
    if (!IsInputKeyDown(EKeys::J))
        return;
#else
    // 패키징 버전에서는 ESC 키를 사용
    if (!IsInputKeyDown(EKeys::Escape))
        return;
#endif

    // 인벤토리 HUD가 표시 중인지 확인
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (ANS_InventoryHUD* IvHUD = Cast<ANS_InventoryHUD>(PC->GetHUD()))
        {
            // 인벤토리 메뉴가 표시 중이면 숨기고 게임 모드로 전환
            if (IvHUD && IvHUD->bIsMenuVisible)
            {
                IvHUD->HideMenu();
                PC->SetInputMode(FInputModeGameOnly());
                PC->SetShowMouseCursor(false);
                return;
            }
        }
    }

    // 게임 인스턴스를 통해 UI 매니저 접근
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            // 인게임 메뉴가 표시되어 있지 않으면 표시, 아니면 숨김
            if (!UIManager->IsInViewportInGameMenuWidget())
                UIManager->ShowInGameMenuWidget(GetWorld());
            else
                UIManager->HideInGameMenuWidget(GetWorld());
        }
    }
}

/**
 * 추적 사운드 재생 함수
 * 플레이어가 적에게 추적당할 때 사운드를 재생합니다.
 * 블루프린트에서 구현 가능한 네이티브 이벤트입니다.
 */
void ANS_PlayerController::PlayTracked_Implementation()
{
    // 추적 시작 사운드 재생
    UGameplayStatics::PlaySound2D(this, ChaseStartSoundCue);
    UE_LOG(LogTemp, Error, TEXT("소리재생"));
}

/**
 * 게임 오버 처리 함수
 * 플레이어가 사망하거나 게임이 종료될 때 호출됩니다.
 * @param bPlayerSurvived - 플레이어 생존 여부 (true: 생존, false: 사망)
 */
void ANS_PlayerController::HandleGameOver(bool bPlayerSurvived)
{
    UE_LOG(LogTemp, Warning, TEXT(">> HandleGameOver 진입 (bPlayerSurvived: %s)"), bPlayerSurvived ? TEXT("true") : TEXT("false"));

    // 입력 무시 설정
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    
    // 마우스 커서 표시
    bShowMouseCursor = true;

    // UI 모드로 전환
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    // 싱글플레이어 모드에서는 게임 시간 정지
    if (GetWorld()->GetNumPlayerControllers() == 1)
    {
        UE_LOG(LogTemp, Warning, TEXT(">> 싱글플레이 - 글로벌 시간 정지"));
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);
    }

    // 게임 인스턴스를 통해 UI 매니저 접근
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            UE_LOG(LogTemp, Warning, TEXT(">> UIManager 유효. 위젯 표시 요청"));
            // 게임 오버 위젯 표시
            UIManager->ShowGameOverWidget(GetWorld());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT(">> UIManager is nullptr"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT(">> GameInstance 캐스팅 실패"));
    }
}

/**
 * 피격 효과 표시 함수 (클라이언트 RPC)
 * 플레이어가 데미지를 입었을 때 화면에 피격 효과를 표시합니다.
 */
void ANS_PlayerController::Client_ShowHitEffect_Implementation()
{
    // 게임 인스턴스를 통해 UI 매니저 접근
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            // 피격 효과 위젯 표시
            UIManager->ShowHitEffectWidget(GetWorld());
        }
    }
}

/**
 * 팁 HUD 업데이트 함수
 * 플레이어에게 팁 메시지를 표시합니다.
 * @param Message - 표시할 팁 메시지
 */
void ANS_PlayerController::UpdateTipHUD(const FText& Message)
{
    // 게임 인스턴스를 통해 UI 매니저 접근
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = GI->GetUIManager())
        {
            if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
            {
                // 팁 텍스트 설정
                PlayerHUD->SetTipText(Message);
            }
        }
    }
}