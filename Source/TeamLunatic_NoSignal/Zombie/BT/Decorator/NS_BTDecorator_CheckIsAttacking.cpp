// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckIsAttacking.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CheckIsAttacking::UNS_BTDecorator_CheckIsAttacking()
{
	NodeName = "CheckIsAttacking";
}

bool UNS_BTDecorator_CheckIsAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                  uint8* NodeMemory) const
{
	bool bIsAttacking = OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsAttacking");
	return !bIsAttacking;
}
