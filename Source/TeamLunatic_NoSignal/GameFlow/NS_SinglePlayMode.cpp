#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "EngineUtils.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
	
}

void ANS_SinglePlayMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[ANS_SinglePlayMode] BeginPlay()"));

	SpawnZombies();
	SpawnItems();
}

void ANS_SinglePlayMode::SpawnZombies()
{

}

void ANS_SinglePlayMode::SpawnItems()
{

}

void ANS_SinglePlayMode::HandleGameOver(bool bPlayerSurvived)
{
	if (bIsGameOver) return;
	bIsGameOver = true;

	if (bPlayerSurvived)
	{
		UE_LOG(LogTemp, Log, TEXT("GAME CLEAR!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("GAME OVER! Player died."));
	}

	// 이후 RestartLevel() or Quit 등 추가 가능
}
