#include "NS_MultiPlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "NS_GameInstance.h"
#include "NS_GameState.h"
#include "NS_PlayerState.h"

ANS_MultiPlayMode::ANS_MultiPlayMode()
{

}

void ANS_MultiPlayMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[ANS_MultiPlayMode] BeginPlay()"));

	SpawnAllPlayers();
}

void ANS_MultiPlayMode::SpawnAllPlayers()
{
	TArray<AActor*> StartPoints;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), StartPoints);

	int32 Index = 0;
	if (ANS_GameState* GameState = Cast<ANS_GameState>(GetWorld()->GetGameState()))
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (ANS_PlayerState* PlayerState = Cast<ANS_PlayerState>(PS))
			{
				APlayerController* PlayerController = PlayerState->GetPlayerController();
				PlayerState->LoadPlayerData(); // 플레이어 상태에서 데이터 로드

				if (PlayerController && StartPoints.IsValidIndex(Index))
				{
					FTransform SpawnTransform = StartPoints[Index]->GetActorTransform();

					//경로로 부터 BP 가져오기
					ConstructorHelpers::FClassFinder<APawn> PawnBPClass(*PlayerState->GetPlayerModelPath());
					if (PawnBPClass.Succeeded())
					{
						PlayerPawnClass = PawnBPClass.Class;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to load player pawn class: %s"), *PlayerState->GetPlayerModelPath());
						continue; // 다음 플레이어로 넘어감
					}

					APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnClass, SpawnTransform);

					if (NewPawn)
					{
						PlayerController->Possess(NewPawn);
						UE_LOG(LogTemp, Log, TEXT("Spawned & Possessed Pawn: %s for Player: %s"), *NewPawn->GetName(), *PlayerState->GetPlayerName());
					}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerState to ANS_PlayerState!"));
			}
			Index++;
		}
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && StartPoints.IsValidIndex(Index))
		{
			FTransform SpawnTransform = StartPoints[Index]->GetActorTransform();
			APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnClass, SpawnTransform);
			if (NewPawn)
			{
				PC->Possess(NewPawn);
			}
			Index++;
		}
	}
}
