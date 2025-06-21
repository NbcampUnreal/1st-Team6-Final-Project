// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NS_Service_RandomLocation.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Service_RandomLocation : public UBTService
{
	GENERATED_BODY()
public:
	UNS_Service_RandomLocation();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ANavigationData* Filter;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector RandomLocationKey;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Random")
	float Radius = 50;
};
