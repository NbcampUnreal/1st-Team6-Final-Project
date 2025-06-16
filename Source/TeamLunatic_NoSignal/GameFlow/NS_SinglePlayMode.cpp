#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "NS_GameInstance.h"
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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn 스폰 실패"));
	}
}

void ANS_SinglePlayMode::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
	if (bIsGameOver)
	{
		return;
	}
	if (DeadCharacter)
	{
		if (DeadCharacter->IsPlayerControlled())
		{
			bIsGameOver = true;
			HandleGameOver(false, EEscapeRoute::None);
		}

	}
}



void ANS_SinglePlayMode::HandleGameOver(bool bPlayerSurvived, EEscapeRoute EscapeRoute)
{
	CurrentEscapeRoute = EscapeRoute;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		// 입력 차단
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);

		// UI 전용 모드 설정
		PC->bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	// 사운드 전체 정지 
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);

	if (bPlayerSurvived)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME CLEAR! Route: %s"),
			*UEnum::GetValueAsString(CurrentEscapeRoute));
	}
	else
	{
		if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
		{
			if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
			{
				UIManager->ShowGameOverWidget(GetWorld());
			}

		}
	}
}



FVector ANS_SinglePlayMode::GetPlayerLocation_Implementation() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;
}

