// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NS_Service_CheckLastSeenLocation.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Service_CheckLastSeenLocation : public UBTService
{
	GENERATED_BODY()
	UNS_Service_CheckLastSeenLocation();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	float AllowRange = 200.f;
};
