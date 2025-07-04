#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/NS_ItemDataStruct.h"
#include "NS_ItemSpawnManager.generated.h"

class APickup;
class USphereComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ItemSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	ANS_ItemSpawnManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SpawnRadiusSphere;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TMap<EEndingType, FName> EndingItemSpawnTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TMap<EEndingType, int32> EndingItemsToSpawn;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	int32 SpawnItemNum;

	//필드에서 아이템이 랜덤 위치로 생성되게 설정.
	UFUNCTION(BlueprintCallable)
	void SpawnItemsInRandomLocations(float Radius);

	void SpawnRandomItemAt(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable)
	void SpawnRandomTaggedLocations();

	UFUNCTION(BlueprintCallable)
	void SpawnEndingItems();

	void SpawnSpecificItemAt(FName ItemID, const FTransform& SpawnTransform);
};
