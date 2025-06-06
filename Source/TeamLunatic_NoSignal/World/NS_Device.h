// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "NS_Device.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_Device : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANS_Device();

	UPROPERTY(VisibleInstanceOnly, Category = "Device | Interaction")
	FInteractableData InstanceInteractableData;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
