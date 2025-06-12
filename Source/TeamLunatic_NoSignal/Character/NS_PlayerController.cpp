#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NS_UIManager.h"

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

