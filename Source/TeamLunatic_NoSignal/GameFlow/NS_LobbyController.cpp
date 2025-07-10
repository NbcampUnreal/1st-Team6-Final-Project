#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFlow/NS_GameInstance.h"
#include "GameFlow/NS_MultiPlayMode.h"

void ANS_LobbyController::BeginPlay()
{
	Super::BeginPlay();

	// 마우스 커서 표시
	bShowMouseCursor = true;

	// UI 전용 모드로 입력 변경
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr); // 필요 시 ReadyUI의 버튼 지정 가능
	SetInputMode(InputMode);

	// 카메라 고정 (0.5초 딜레이 추가)
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 0.5f); // 0.5초 딜레이
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
	// OnPossess 시 카메라 고정 (0.5초 딜레이 추가)
	// (폰 카메라가 잠시 잡히는 것을 방지)
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 1.0f); // 0.5초 딜레이
			break;
		}
	}
}

void ANS_LobbyController::Client_ShowWait_Implementation()
{
	// Client_ShowWait 완전 비활성화 - 중복 로딩 스크린 방지
	UE_LOG(LogTemp, Error, TEXT("=== Client_ShowWait 호출됨 - 완전 비활성화됨 ==="));

	// 아무것도 하지 않음 (NS_LoadingScreen은 다른 곳에서 관리)
	return;
}