// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_Attackable.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_Attackable::UNS_BTDecorator_Attackable()
{
	NodeName = "Attackable";
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
}

bool UNS_BTDecorator_Attackable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	
	bool bIsAttackable = BlackboardComponent->GetValueAsBool(GetSelectedBlackboardKey());
	return bIsAttackable;
}
