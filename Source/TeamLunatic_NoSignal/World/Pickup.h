// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "Pickup.generated.h"

class UDataTable;
class UItemBase;
class ANS_PlayerCharacter;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	APickup();

	//void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
	//void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);

	//FORCEINLINE UItemBase* GetItemData() { return ItemReference; };

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemDataBase")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemDataBase")
	FName DesiredItemID;
	
	//UPROPERTY(VisibleAnywhere, Category = "Pickup | ItemReference")
	//UItemBase* ItemReference;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemReference")
	int32 ItemQuantity;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;
	
	virtual void BeginPlay() override;
	virtual void Interact() override;
	void TakePickup(const ANS_PlayerCharacter* Taker);
};
