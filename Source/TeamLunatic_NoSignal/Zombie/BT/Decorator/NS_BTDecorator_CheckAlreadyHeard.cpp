// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckAlreadyHeard.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CheckAlreadyHeard::UNS_BTDecorator_CheckAlreadyHeard()
{
	NodeName = "CheckAlreadyHeard";
}

bool UNS_BTDecorator_CheckAlreadyHeard::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	bool CheckAlreadyHeard = BlackboardComp->GetValueAsBool(BlackboardKey.SelectedKeyName);
	return !CheckAlreadyHeard;
}
