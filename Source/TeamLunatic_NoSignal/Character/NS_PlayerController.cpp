#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_Msg_GameOver.h" 
#include "UI/NS_PlayerHUD.h"
#include "Blueprint/UserWidget.h" 
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


    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
      //GI->GetUIManager()->CompleteLoadingProcess();
      //  GI->GetUIManager()->OnLoadingFinished.BindLambda([GI]()
      //  {
      //     UE_LOG(LogTemp, Warning, TEXT("NS_PlayerController CloseLoadingUI "));
      //     GI->GetUIManager()->CloseLoadingUI();
      //     GI->GetUIManager()->ShowPlayerHUDWidget(GI->GetWorld());
      //  });//로딩스크린 위젯 띄워서  로딩바 몇초 움직이고 OpenLevel()하는 방식.




        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, GI]()
        {
            // asybc loading screen PlugIn 활용할때 사용하는 로직.
            GI->GetUIManager()->ShowPlayerHUDWidget(GI->GetWorld());
        }, 1.f, false);
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
    UE_LOG(LogTemp, Error, TEXT("소리재생"));
}

void ANS_PlayerController::HandleGameOver(bool bPlayerSurvived)
{
    UE_LOG(LogTemp, Warning, TEXT(">> HandleGameOver 진입 (bPlayerSurvived: %s)"), bPlayerSurvived ? TEXT("true") : TEXT("false"));

    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    if (GetWorld()->GetNumPlayerControllers() == 1)
    {
        UE_LOG(LogTemp, Warning, TEXT(">> 싱글플레이 - 글로벌 시간 정지"));
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);
    }

    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            UE_LOG(LogTemp, Warning, TEXT(">> UIManager 유효. 위젯 표시 요청"));
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

void ANS_PlayerController::Client_ShowHitEffect_Implementation()
{
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
        {
            UIManager->ShowHitEffectWidget(GetWorld());
        }
    }
}

void ANS_PlayerController::UpdateTipHUD(const FText& Message)
{
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = GI->GetUIManager())
        {
            if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
            {
                PlayerHUD->SetTipText(Message);
            }
        }
    }
}