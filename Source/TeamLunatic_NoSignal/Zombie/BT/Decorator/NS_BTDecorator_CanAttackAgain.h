// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "NS_BTDecorator_CanAttackAgain.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTDecorator_CanAttackAgain : public UBTDecorator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector KeySelector;
	UNS_BTDecorator_CanAttackAgain();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
