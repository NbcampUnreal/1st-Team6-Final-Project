// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "Pickup.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	APickup();

	UStaticMeshComponent* PickupMesh;

	UDataTable* ItemDataTable;

	FName DesiredItemID;

	/*UItemBase* ItemReference;*/

	int32 ItemQuantity;

	FInteractableData InstanceInteractableData;

	/*void InitializePickup(const TSubclassOf<UItembase> BaseClass, const int32 InQuantity);*/

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
