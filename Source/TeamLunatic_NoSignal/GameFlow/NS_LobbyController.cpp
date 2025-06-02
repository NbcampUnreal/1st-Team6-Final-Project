#include "NS_LobbyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"

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
	if (APlayerState* PS = GetPlayerState<APlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyController] Server_RequestStartGame called by PlayerId=%d, Name=%s"),
			PS->GetPlayerId(), *PS->GetPlayerName());

		if (PS->GetPlayerId() == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[LobbyController] Host verified. Starting level..."));

			if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
			{
				UGameplayStatics::OpenLevel(this, FName("MainLevel"));
			}
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
