// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_BTDecorator_IsDetecting.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UNS_BTDecorator_IsDetecting::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool IsDetecting = OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsDetecting");
	return IsDetecting;
}
