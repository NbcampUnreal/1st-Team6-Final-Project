// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckHit.h"

#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CheckHit::UNS_BTDecorator_CheckHit()
{
	NodeName = "CheckHit";
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
}

bool UNS_BTDecorator_CheckHit::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	return bResult;
}
