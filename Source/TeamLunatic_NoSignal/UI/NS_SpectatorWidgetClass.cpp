#include "UI/NS_SpectatorWidgetClass.h"
#include "GameFlow/NS_PlayerState.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
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
}

void UNS_SpectatorWidgetClass::OnLeftClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	TArray<APlayerState*> AlivePlayers = GS->PlayerArray;
	AlivePlayers.RemoveAll([](APlayerState* PS) {
		return PS->IsOnlyASpectator() || !PS->GetPawn();
	});

	if (AlivePlayers.Num() == 0) return;

	CurrentIndex = (CurrentIndex - 1 + AlivePlayers.Num()) % AlivePlayers.Num();
	SpectatePlayerAtIndex(CurrentIndex);
}

void UNS_SpectatorWidgetClass::OnRightClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	TArray<APlayerState*> AlivePlayers = GS->PlayerArray;
	AlivePlayers.RemoveAll([](APlayerState* PS) {
		return PS->IsOnlyASpectator() || !PS->GetPawn();
	});

	if (AlivePlayers.Num() == 0) return;

	CurrentIndex = (CurrentIndex + 1) % AlivePlayers.Num();
	SpectatePlayerAtIndex(CurrentIndex);
}

void UNS_SpectatorWidgetClass::SpectatePlayerAtIndex(int32 Index)
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	TArray<APlayerState*> AlivePlayers = GS->PlayerArray;
	AlivePlayers.RemoveAll([](APlayerState* PS) {
		return PS->IsOnlyASpectator() || !PS->GetPawn();
	});

	if (!AlivePlayers.IsValidIndex(Index)) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	APlayerState* TargetState = AlivePlayers[Index];
	APawn* TargetPawn = TargetState->GetPawn();

	if (!PC || !TargetPawn) return;

	PC->SetViewTargetWithBlend(TargetPawn, 0.3f);

	if (PlayerName)
	{
		if (const ANS_PlayerState* PS = Cast<ANS_PlayerState>(TargetState))
		{
			const FString Label = FString::Printf(TEXT("Player %d"), PS->PlayerIndex + 1);
			PlayerName->SetText(FText::FromString(Label));
		}
		else
		{
			PlayerName->SetText(FText::FromString(TEXT("Unknown")));
		}
	}
}

void UNS_SpectatorWidgetClass::OnQuitClicked()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->ClientTravel("/Game/Maps/MainTitle", ETravelType::TRAVEL_Absolute);
	}
}

