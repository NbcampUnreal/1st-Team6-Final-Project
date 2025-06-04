// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckAttackable.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


UNS_BTDecorator_CheckAttackable::UNS_BTDecorator_CheckAttackable()
{
	NodeName = "CheckAttackable";
}

bool UNS_BTDecorator_CheckAttackable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                 uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	
	bool bIsAttackable = BlackboardComponent->GetValueAsBool(BlackboardKey.SelectedKeyName);
	return bIsAttackable;
}
