#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "Inventory UI/NS_InventoryHUD.h"

ANS_PlayerController::ANS_PlayerController()
{
    bShowMouseCursor = false; // false로 시작 (InGameMenu에서는 true로 전환)
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ANS_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    if (!IsLocalController()) return; // 서버일 경우 바로 반환해서 UI 안 띄움

    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            UIManager->ShowPlayerHUDWidget(GetWorld());
        }
    }
}

void ANS_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("ToggleInGameMenu", IE_Pressed, this, &ANS_PlayerController::ToggleInGameMenu);
        InputComponent->BindAction("TestGameOver", IE_Pressed, this, &ANS_PlayerController::TestGameOver);
        InputComponent->BindAction("TestGameMsg", IE_Pressed, this, &ANS_PlayerController::TestGameMsg);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("InputComponent is null in SetupInputComponent!"));
}

//이함수는 단축키"M"으로 게임중에 UI를 띄우는 함수입니다.
//ProjectSetting>Input>입력매핑>추가 "ToggleInGameMenu" (단축키"M"설정)
void ANS_PlayerController::ToggleInGameMenu()
{
#if WITH_EDITOR
    // 에디터에서는 M 일 때 작동
    if (!IsInputKeyDown(EKeys::M))
        return;
#else
    // 패키징 버전에서는 ESC 일 때 작동
    if (!IsInputKeyDown(EKeys::Escape))
        return;
#endif

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (ANS_InventoryHUD* IvHUD = Cast<ANS_InventoryHUD>(PC->GetHUD()))
        {
            if (IvHUD && IvHUD->bIsMenuVisible)
            {
                IvHUD->HideMenu();
                PC->SetInputMode(FInputModeGameOnly());
                PC->SetShowMouseCursor(false);
                return;
            }
        }
    }

    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            if (!UIManager->IsInViewportInGameMenuWidget())
                UIManager->ShowInGameMenuWidget(GetWorld());
            else
                UIManager->HideInGameMenuWidget(GetWorld());
        }
    }
}
void ANS_PlayerController::TestGameOver()
{
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            FString Msg = TEXT("TEST!! HELLOW!~~~~");
            UIManager->ShowGameOverWidget(GetWorld());
        }
    }
}
void ANS_PlayerController::TestGameMsg()
{
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            FString Msg = TEXT("TEST!! HELLOW!~~~~");
            UIManager->ShowGameMsgWidget(Msg, GetWorld());
            // UIManager->ShowGameOverWidget(GetWorld());
        }
    }
}

void ANS_PlayerController::PlayTracked_Implementation()
{
    UGameplayStatics::PlaySound2D(this, ChaseStartSoundCue);

}

// 클라이언트 RPC: 게임 오버 UI 표시
void ANS_PlayerController::ClientShowGameOverUI_Implementation()
{
    // 클라이언트에서만 실행되도록 보장 
    if (!IsLocalController()) // 현재 이 컨트롤러가 로컬 플레이어의 컨트롤러인지 확인
    {
        return;
    }

    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            // UIManager를 통해 게임 오버 위젯 표시
            UIManager->ShowGameOverWidget(GetWorld());
            UE_LOG(LogTemp, Warning, TEXT("ClientShowGameOverUI_Implementation: Game Over Widget 표시 요청됨."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ClientShowGameOverUI_Implementation: UIManager가 유효하지 않습니다."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ClientShowGameOverUI_Implementation: GameInstance가 유효하지 않습니다."));
    }
}

// 클라이언트 RPC: 게임 오버 시 입력 모드 변경 (마우스 활성화, 입력 차단)
void ANS_PlayerController::ClientSetGameOverInputMode_Implementation()
{
    // 클라이언트에서만 실행되도록 보장
    if (!IsLocalController())
    {
        return;
    }

    // 캐릭터 이동 및 조준 입력 비활성화
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);

    // 마우스 커서 표시
    bShowMouseCursor = true;

    // 입력 모드를 UI 전용으로 설정
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스를 뷰포트에 고정하지 않음
    SetInputMode(InputMode); // 새로운 입력 모드 적용

    UE_LOG(LogTemp, Warning, TEXT("ClientSetGameOverInputMode_Implementation: 입력 모드 UIOnly로 설정됨."));
}