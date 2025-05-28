// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NS_Service_LookPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Service_LookPlayer : public UBTService
{
	GENERATED_BODY()
public:
	UNS_Service_LookPlayer();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
