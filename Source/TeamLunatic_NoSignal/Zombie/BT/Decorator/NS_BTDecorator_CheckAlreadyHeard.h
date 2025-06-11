// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "NS_BTDecorator_CheckAlreadyHeard.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTDecorator_CheckAlreadyHeard : public UBTDecorator
{
	GENERATED_BODY()

public:
	UNS_BTDecorator_CheckAlreadyHeard();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FBlackboardKeySelector BlackboardKey;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
