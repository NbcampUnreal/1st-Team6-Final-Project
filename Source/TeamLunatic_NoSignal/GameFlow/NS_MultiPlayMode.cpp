#include "NS_MultiPlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"

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
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PlayerPawnClass && StartPoints.IsValidIndex(Index))
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
