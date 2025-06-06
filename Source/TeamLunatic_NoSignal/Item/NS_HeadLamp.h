// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_HeadLamp.generated.h"

class UStaticMeshComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_HeadLamp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANS_HeadLamp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;
};
