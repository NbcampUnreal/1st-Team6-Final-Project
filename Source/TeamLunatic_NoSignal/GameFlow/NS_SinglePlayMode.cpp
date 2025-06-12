#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Character/NS_PlayerCharacterBase.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
}

void ANS_SinglePlayMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World || AvailablePawnClasses.Num() < 4) return;

	const int32 RandIndex = FMath::RandRange(0, 3);
	TSubclassOf<APawn> ChosenPawnClass = AvailablePawnClasses[RandIndex];

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC || PC->GetPawn()) return;

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
		PC->Possess(NewPawn);
		UE_LOG(LogTemp, Log, TEXT("싱글플레이: 랜덤 인덱스 %d 캐릭터 %s 스폰됨"), RandIndex, *NewPawn->GetName());
	}
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
