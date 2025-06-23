// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_BTDecorator_AttackAgain.h"

#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_AttackAgain::UNS_BTDecorator_AttackAgain()
{
	NodeName = "CanAttackAgain";
	bNotifyTick = true;
}

bool UNS_BTDecorator_AttackAgain::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	bool Attacking = BlackboardComponent->GetValueAsBool(GetSelectedBlackboardKey());
	
	return Attacking;
}
