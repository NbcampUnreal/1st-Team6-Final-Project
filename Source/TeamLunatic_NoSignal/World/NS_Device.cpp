// Fill out your copyright notice in the Description page of Project Settings.


#include "World/NS_Device.h"

// Sets default values
ANS_Device::ANS_Device()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ANS_Device::BeginPlay()
{
	Super::BeginPlay();

	InteractableData.InteractableType = EInteractableType::Device;
	InteractableData.Action = FText::FromString("Open");
	InteractableData.Name = FText::FromString("Security Door");

	InteractableData = InstanceInteractableData;
}

// Called every frame
void ANS_Device::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


