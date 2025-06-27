#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"

/**
 * 게임 시작 시 호출되는 함수
 * 로비 환경 설정 및 UI 초기화를 담당
 */
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
			// Ready UI 표시
			GI->ShowReadyUI();

			// 플레이어 상태 목록 업데이트
			if (GI->ReadyUIInstance)
			{
				GI->ReadyUIInstance->UpdatePlayerStatusList();
			}
		}
	}
}


/**
 * 플레이어 폰 소유 시 호출되는 함수
 * 로비 카메라로 뷰 설정
 */
void ANS_LobbyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 로비 카메라 찾아서 뷰 설정
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 0.0f);
			break;
		}
	}
}

/**
 * 클라이언트에 대기 UI 표시 요청
 * 서버에서 호출하여 모든 클라이언트에 전파
 */
void ANS_LobbyController::Client_ShowWait_Implementation()
{
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		// 대기 UI 표시
		GI->ShowWait();
	}
}
