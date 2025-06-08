#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/NS_ItemDataStruct.h"
#include "NS_ItemSpawnManager.generated.h"

class APickup;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ItemSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	ANS_ItemSpawnManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<APickup> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<EItemType> SpawnableTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning Config")
	FName SpawnPointTagToFind;

public:
	void FindAndSpawnItems();

	void SpawnRandomItemAt(const FTransform& SpawnTransform);
};
