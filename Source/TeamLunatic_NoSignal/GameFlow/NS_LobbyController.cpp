#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFlow/NS_GameInstance.h"
#include "GameFlow/NS_MultiPlayMode.h"
#include "UI/NS_UIManager.h"

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
			SetViewTargetWithBlend(*It, 0.5f); // 0.5초 딜레이
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

void ANS_LobbyController::Server_NotifyLoadingComplete_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 로딩 완료 알림 받음: %s"), *GetName());

	// MultiPlayMode에게 이 플레이어가 로딩 완료되었음을 알림
	if (ANS_MultiPlayMode* MultiMode = Cast<ANS_MultiPlayMode>(GetWorld()->GetAuthGameMode()))
	{
		MultiMode->OnPlayerLoadingComplete(this);
	}
}

void ANS_LobbyController::Client_HideLoadingScreen_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("서버로부터 로딩 스크린 숨기기 명령 받음"));

	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			UIManager->HideLoadingScreen(GetWorld());
			UE_LOG(LogTemp, Log, TEXT("동기화된 로딩 스크린 숨김"));
		}
	}
}