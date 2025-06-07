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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Device")
	UStaticMeshComponent* DoorA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Device")
	UStaticMeshComponent* DoorB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Device")
	UStaticMeshComponent* Frame;
	UPROPERTY(EditInstanceOnly, Category = "Device | Interaction")
	FInteractableData InstanceInteractableData;
	UPROPERTY(ReplicatedUsing = OnRep_InteractableData)
	FInteractableData InteractableData;
	UFUNCTION()
	void OnRep_InteractableData();

	virtual void BeginFocus() override;

	virtual void EndFocus()override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateInteractableData();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
