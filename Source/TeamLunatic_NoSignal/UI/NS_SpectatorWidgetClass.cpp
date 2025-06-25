#include "UI/NS_SpectatorWidgetClass.h"
#include "GameFlow/NS_PlayerState.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

void UNS_SpectatorWidgetClass::NativeConstruct()
{
	Super::NativeConstruct();

	if (LeftButton)
		LeftButton->OnClicked.AddDynamic(this, &UNS_SpectatorWidgetClass::OnLeftClicked);

	if (RightButton)
		RightButton->OnClicked.AddDynamic(this, &UNS_SpectatorWidgetClass::OnRightClicked);

	if (QuitButton)
		QuitButton->OnClicked.AddDynamic(this, &UNS_SpectatorWidgetClass::OnQuitClicked);

	UpdateAndSpectateFirstPlayer();
}

void UNS_SpectatorWidgetClass::UpdateAlivePlayers()
{
	AlivePlayerStates.Empty();

	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS) return;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (IsValid(PS) && !PS->IsOnlyASpectator() && IsValid(PS->GetPawn()))
		{
			AlivePlayerStates.Add(PS);
		}
	}
}

void UNS_SpectatorWidgetClass::OnLeftClicked()
{
	UpdateAlivePlayers(); 
	if (AlivePlayerStates.Num() == 0) return;

	CurrentIndex = (CurrentIndex - 1 + AlivePlayerStates.Num()) % AlivePlayerStates.Num();
	SpectatePlayerAtIndex(CurrentIndex);
}

void UNS_SpectatorWidgetClass::OnRightClicked()
{
	UpdateAlivePlayers(); 
	if (AlivePlayerStates.Num() == 0) return;

	CurrentIndex = (CurrentIndex + 1) % AlivePlayerStates.Num();
	SpectatePlayerAtIndex(CurrentIndex);
}

void UNS_SpectatorWidgetClass::SpectatePlayerAtIndex(int32 Index)
{
	if (!AlivePlayerStates.IsValidIndex(Index)) return;

	APlayerController* PC = GetOwningPlayer();
	APlayerState* TargetState = AlivePlayerStates[Index];

	if (!PC || !TargetState) return;

	APawn* TargetPawn = TargetState->GetPawn();
	if (!TargetPawn)
	{
		UpdateAndSpectateFirstPlayer();
		return;
	}

	PC->SetViewTargetWithBlend(TargetPawn, 0.3f);

	if (PlayerName)
	{
		if (const ANS_PlayerState* NS_PS = Cast<ANS_PlayerState>(TargetState))
		{
			const FString Label = FString::Printf(TEXT("Player %d"), NS_PS->PlayerIndex + 1);
			PlayerName->SetText(FText::FromString(Label));
		}
		else
		{
			PlayerName->SetText(FText::FromString(TargetState->GetPlayerName()));
		}
	}
}

void UNS_SpectatorWidgetClass::UpdateAndSpectateFirstPlayer()
{
	UpdateAlivePlayers();
	if (AlivePlayerStates.Num() > 0)
	{
		CurrentIndex = 0;
		SpectatePlayerAtIndex(CurrentIndex);
	}
	else
	{
		if (PlayerName) PlayerName->SetText(FText::FromString(TEXT("No players to spectate")));
	}
}


void UNS_SpectatorWidgetClass::OnQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ClientTravel("/Game/Maps/MainTitle", ETravelType::TRAVEL_Absolute);
	}
}