#include "NS_SinglePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Character/NS_PlayerCharacterBase.h"
//#include "Zombie/ZombieSpawnManager.h"
//#include "Item/ItemSpawnManager.h"

ANS_SinglePlayMode::ANS_SinglePlayMode()
{
	// PlayerController와 Pawn은 직접 지정하거나 Editor에서 세팅
}

void ANS_SinglePlayMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[ANS_SinglePlayMode] BeginPlay()"));

	SpawnPlayer();
	SpawnTrackerZombie();
	//SpawnGeneralZombies();
	//SpawnEscapeItems();
	//SpawnHealingItems();
}

void ANS_SinglePlayMode::SpawnPlayer()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !PlayerPawnClass) return;

	APlayerStart* Start = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()));
	if (!Start) return;

	CachedPlayerStart = Start;

	APawn* PlayerPawn = GetWorld()->SpawnActor<APawn>(PlayerPawnClass, Start->GetActorLocation(), Start->GetActorRotation());
	if (PlayerPawn)
	{
		PC->Possess(PlayerPawn);
		UE_LOG(LogTemp, Log, TEXT("Player spawned and possessed."));
	}
}

void ANS_SinglePlayMode::SpawnTrackerZombie()
{
	if (!TrackerZombieClass) return;

	// 맵에 배치된 4개 지점 중 하나 선택 (Tag로 식별)
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsWithTag(this, FName("TrackerSpawn"), SpawnPoints);

	if (SpawnPoints.Num() > 0)
	{
		AActor* ChosenPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
		TrackerZombieInstance = GetWorld()->SpawnActor<AActor>(TrackerZombieClass, ChosenPoint->GetActorLocation(), ChosenPoint->GetActorRotation());

		if (TrackerZombieInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Tracker zombie spawned at: %s"), *ChosenPoint->GetName());
		}
	}
}

//void ANS_SinglePlayMode::SpawnGeneralZombies()
//{
//	for (TActorIterator<AZombieSpawnManager> It(GetWorld()); It; ++It)
//	{
//		ZombieSpawnManager = *It;
//		break;
//	}
//
//	if (ZombieSpawnManager)
//	{
//		ZombieSpawnManager->SpawnZombies(); // 매니저 내부에서 SpawnPoint 처리
//		UE_LOG(LogTemp, Log, TEXT("Requested general zombie spawn from manager."));
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("ZombieSpawnManager not found!"));
//	}
//}
//
//void ANS_SinglePlayMode::SpawnEscapeItems()
//{
//	for (TActorIterator<AItemSpawnManager> It(GetWorld()); It; ++It)
//	{
//		ItemSpawnManager = *It;
//		break;
//	}
//
//	if (ItemSpawnManager && EscapeItemTable)
//	{
//		ItemSpawnManager->DistributeEscapeItems(EscapeItemTable);
//		UE_LOG(LogTemp, Log, TEXT("Escape items spawned."));
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("ItemSpawnManager or EscapeItemTable is null!"));
//	}
//}
//
//void ANS_SinglePlayMode::SpawnHealingItems()
//{
//	if (ItemSpawnManager && HealingItemClass)
//	{
//		ItemSpawnManager->SpawnHealingItems(HealingItemClass, HealingItemCount);
//		UE_LOG(LogTemp, Log, TEXT("Healing items spawned."));
//	}
//}

void ANS_SinglePlayMode::RegisterLoudNoise(FVector SoundOrigin)
{
	UE_LOG(LogTemp, Log, TEXT("Loud noise registered at: %s"), *SoundOrigin.ToString());
	BroadcastNoiseLocation(SoundOrigin);
}

void ANS_SinglePlayMode::BroadcastNoiseLocation(const FVector& Location)
{
	if (TrackerZombieInstance)
	{
		// 예: NotifyNoise(Location) 같은 함수가 좀비에 구현되어 있어야 함
		// Cast<AYourZombieClass>(TrackerZombieInstance)->NotifyNoise(Location);
	}
}

void ANS_SinglePlayMode::TryEscape(const FEscapeAttemptInfo& Info)
{
	//if (!Info.Player) return;

	//const TArray<FName>& Items = Info.Player->GetInventory(); // 인벤토리 함수가 있다고 가정

	//if (Info.EscapeTargetTag == "Car" && CheckCarEscapeCondition(Items))
	//{
	//	HandleGameOver(true, EEscapeRoute::Car);
	//}
	//else if (Info.EscapeTargetTag == "Radio" && CheckRadioEscapeCondition(Items))
	//{
	//	HandleGameOver(true, EEscapeRoute::Radio);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Escape conditions not met."));
	//}
}

bool ANS_SinglePlayMode::CheckCarEscapeCondition(const TArray<FName>& PlayerItems) const
{
	return PlayerItems.Contains("CarKey") &&
		PlayerItems.FilterByPredicate([](const FName& Item) { return Item == "Fuel"; }).Num() >= 2;
}

bool ANS_SinglePlayMode::CheckRadioEscapeCondition(const TArray<FName>& PlayerItems) const
{
	return PlayerItems.FilterByPredicate([](const FName& Item) { return Item == "Battery"; }).Num() >= 2 &&
		PlayerItems.Contains("Antenna");
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

	// TODO: UI 호출, 레벨 리셋 등 후처리
}
