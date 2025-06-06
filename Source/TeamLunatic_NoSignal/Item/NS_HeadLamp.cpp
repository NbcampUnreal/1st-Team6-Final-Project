// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Item/NS_HeadLamp.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"

ANS_HeadLamp::ANS_HeadLamp()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	HeadLamp = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadLamp"));
	HeadLamp->SetupAttachment(MeshComponent);
	HeadLamp->SetIntensity(10000.0f);
	HeadLamp->SetLightColor(FLinearColor::Yellow);
	HeadLamp->SetInnerConeAngle(15.0f);
	HeadLamp->SetOuterConeAngle(30.0f);
	HeadLamp->SetAttenuationRadius(1000.0f);


}
