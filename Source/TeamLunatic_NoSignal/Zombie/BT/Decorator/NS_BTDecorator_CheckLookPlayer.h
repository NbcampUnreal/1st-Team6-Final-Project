// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "NS_BTDecorator_CheckLookPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTDecorator_CheckLookPlayer : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UNS_BTDecorator_CheckLookPlayer();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
