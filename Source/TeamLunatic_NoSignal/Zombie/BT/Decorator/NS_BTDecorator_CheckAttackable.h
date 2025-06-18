// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "NS_BTDecorator_CheckAttackable.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTDecorator_CheckAttackable : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	FBlackboardKeySelector KeySelector;
	
public:
	UNS_BTDecorator_CheckAttackable();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
