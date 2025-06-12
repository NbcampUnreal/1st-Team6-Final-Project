#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
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

	UWorld* World = GetWorld();
	if (!World || !NewPlayer || AvailablePawnClasses.Num() < 1) return;

	// 기존 Pawn 제거
	if (APawn* ExistingPawn = NewPlayer->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	// 랜덤 선택
	const int32 RandIndex = FMath::RandRange(0, AvailablePawnClasses.Num() - 1);
	TSubclassOf<APawn> ChosenPawnClass = AvailablePawnClasses[RandIndex];

	// 스폰 위치
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
		UE_LOG(LogTemp, Log, TEXT("🎲 랜덤 인덱스 %d 캐릭터 %s 스폰됨"), RandIndex, *NewPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Pawn 스폰 실패"));
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
