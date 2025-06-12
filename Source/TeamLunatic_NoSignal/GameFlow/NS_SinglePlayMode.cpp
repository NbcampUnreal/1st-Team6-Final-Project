#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
}

void ANS_SinglePlayMode::BeginPlay()
{
	Super::BeginPlay();
}

void ANS_SinglePlayMode::HandleGameOver(bool bPlayerSurvived, EEscapeRoute EscapeRoute)
{
	if (bIsGameOver) return;
	bIsGameOver = true;
	CurrentEscapeRoute = EscapeRoute;

	if (bPlayerSurvived)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME CLEAR! Route: %s"),
			*UEnum::GetValueAsString(CurrentEscapeRoute));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER! Player died."));
	}

}

FVector ANS_SinglePlayMode::GetPlayerLocation_Implementation() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;
}