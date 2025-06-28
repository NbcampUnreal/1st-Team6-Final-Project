#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"

void ANS_LobbyController::BeginPlay()
{
	Super::BeginPlay();

	//  마우스 커서 표시
	bShowMouseCursor = true;

	//  UI 전용 모드로 입력 변경
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr); // 필요 시 ReadyUI의 버튼 지정 가능
	SetInputMode(InputMode);

	//  카메라 고정
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 0.0f);
			break;
		}
	}

	if (IsLocalController())
	{
		if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
		{
			GI->ShowReadyUI();

			if (GI->ReadyUIInstance)
			{
				GI->ReadyUIInstance->UpdatePlayerStatusList();
			}
		}
	}
}


void ANS_LobbyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 0.0f);
			break;
		}
	}
}

void ANS_LobbyController::Client_ShowWait_Implementation()
{
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		GI->ShowWait();
	}
}

void ANS_LobbyController::Client_ShowLoadingScreen_Implementation()
{
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			// Ready UI 숨기기
			GI->HideReadyUI();

			// 로딩 스크린 표시
			UIManager->ShowLoadingScreen(GetWorld());

			UE_LOG(LogTemp, Log, TEXT("멀티플레이 로딩 스크린 표시"));
		}
	}
}
