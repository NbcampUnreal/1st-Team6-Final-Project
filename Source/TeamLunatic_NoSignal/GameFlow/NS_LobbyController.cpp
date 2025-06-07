#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "NS_PlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"



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
}


void ANS_LobbyController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("StartGame", IE_Pressed, this, &ANS_LobbyController::HandleStartGame);
	}
}

void ANS_LobbyController::HandleStartGame()
{
	UE_LOG(LogTemp, Log, TEXT("[LobbyController] Enter key pressed on client."));

	if (!HasAuthority())
	{
		Server_RequestStartGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyController] Unexpected: client has authority?"));
	}
}

void ANS_LobbyController::Server_RequestStartGame_Implementation()
{
	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PlayerState))
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyController] Server_RequestStartGame called by PlayerId=%d, Name=%s"),
			PS->GetPlayerId(), *PS->GetPlayerName());

		if (PS->PlayerIndex == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[LobbyController] Host verified. Starting level with GameMode..."));

			const FString LevelPath = TEXT("/Game/Maps/MainWorld");
			const FString Options = TEXT("Game=/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C");

			GetWorld()->ServerTravel(LevelPath + TEXT("?") + Options);
		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[LobbyController] PlayerId=%d is not host. Ignoring start request."), PS->GetPlayerId());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LobbyController] No valid PlayerState on server."));
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
