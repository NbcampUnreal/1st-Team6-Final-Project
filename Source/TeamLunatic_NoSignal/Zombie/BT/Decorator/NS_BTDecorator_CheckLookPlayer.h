// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "NS_BTDecorator_CheckLookPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTDecorator_CheckLookPlayer : public UBTDecorator
{
	GENERATED_BODY()
public:
	UNS_BTDecorator_CheckLookPlayer();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector KeySelector;
	UPROPERTY(EditAnywhere, Category = "Condition")
	float AcceptableViewAngle;
};
