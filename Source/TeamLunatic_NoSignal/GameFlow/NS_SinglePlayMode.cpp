#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "Character/NS_PlayerController.h"
#include "Engine/World.h"
#include "Character/NS_PlayerCharacterBase.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ANS_PlayerController::StaticClass();
}


void ANS_SinglePlayMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UNS_GameInstance* GI = GetGameInstance<UNS_GameInstance>();
	UWorld* World = GetWorld();

	if (!World || !NewPlayer || !GI || GI->AvailableCharacterClasses.Num() < 1) return;


	if (APawn* ExistingPawn = NewPlayer->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	const int32 RandIndex = FMath::RandRange(0, GI->AvailableCharacterClasses.Num() - 1);
	TSubclassOf<APawn> ChosenPawnClass = GI->AvailableCharacterClasses[RandIndex];

	AActor* Start = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass());
	if (!Start) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* NewPawn = World->SpawnActor<APawn>(
		ChosenPawnClass,
		Start->GetActorLocation(),
		Start->GetActorRotation(),
		Params
	);

	if (NewPawn)
	{
		NewPlayer->Possess(NewPawn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn 스폰 실패"));
	}
}

void ANS_SinglePlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
	if (bIsGameOver || !DeadCharacter) return;

	if (DeadCharacter->IsPlayerControlled())
	{
		if (APlayerController* PC = DeadCharacter->GetController<APlayerController>())
		{
			if (ANS_PlayerController* NS_PC = Cast<ANS_PlayerController>(PC))
			{
				NS_PC->HandleGameOver(false);
			}
		}

		bIsGameOver = true;
	}
}


FVector ANS_SinglePlayMode::GetPlayerLocation_Implementation() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;
}