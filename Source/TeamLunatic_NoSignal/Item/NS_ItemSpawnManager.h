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

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning", meta = (DisplayName = "Execute Spawning Logic"))
	void K2_ExecuteSpawning();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<APickup> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<EItemType> SpawnableTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning Config")
	FName SpawnPointTagToFind;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	int32 SpawnItemNum;

	//필드에서 아이템이 랜덤 위치로 생성되게 설정.
	UFUNCTION(BlueprintCallable)
	void SpawnItemsInRandomLocations(float Radius);

	void FindAndSpawnItems();

	void SpawnRandomItemAt(const FTransform& SpawnTransform);
};
