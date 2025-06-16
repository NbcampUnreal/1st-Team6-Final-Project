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

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(FName("LobbyCamera")))
		{
			SetViewTargetWithBlend(*It, 0.5f);
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
			SetViewTargetWithBlend(*It, 0.3f);
			break;
		}
	}
}
