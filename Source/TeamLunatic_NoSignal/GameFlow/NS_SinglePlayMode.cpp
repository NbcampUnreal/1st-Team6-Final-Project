#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_MainGamePlayerState.h" 
#include "Engine/World.h"
#include "Character/NS_PlayerCharacterBase.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ANS_PlayerController::StaticClass();
	// 싱글플레이 모드에서는 플레이어 수를 1로 설정
	PlayerCount = 1;
}


void ANS_SinglePlayMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UWorld* World = GetWorld();
	if (!World || !NewPlayer) return;

	if (SinglePlayPawnClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SinglePlayPawnClasses 배열이 비어있습니다. BP_NS_SinglePlayMode에서 폰 클래스를 할당해주세요!"));
		return;
	}

	if (APawn* ExistingPawn = NewPlayer->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	const int32 RandIndex = FMath::RandRange(0, SinglePlayPawnClasses.Num() - 1);
	TSubclassOf<APawn> ChosenPawnClass = SinglePlayPawnClasses[RandIndex];

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
	if (bIsGameOver || !DeadCharacter)
	{
		return;
	}

	if (DeadCharacter->IsPlayerControlled())
	{
		if (AController* Controller = DeadCharacter->GetController())
		{
			if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
			{
				PS->bIsAlive = false;

				if (GetNetMode() == NM_Standalone)
				{
					PS->OnRep_IsAlive();
				}
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

void ANS_SinglePlayMode::BeginPlay()
{
	Super::BeginPlay();

	// 싱글플레이 모드 설정
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		GI->SetGameModeType(EGameModeType::SinglePlayMode);
	}
}


