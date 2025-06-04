// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CanAttackAgain.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CanAttackAgain::UNS_BTDecorator_CanAttackAgain()
{
}

bool UNS_BTDecorator_CanAttackAgain::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	
	bool bCanAttackAgain = BlackboardComponent->GetValueAsBool(KeySelector.SelectedKeyName);
	return bCanAttackAgain;
}
