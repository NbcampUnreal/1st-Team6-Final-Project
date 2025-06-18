// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckAttackable.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


UNS_BTDecorator_CheckAttackable::UNS_BTDecorator_CheckAttackable()
{
	NodeName = "CheckAttackable";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNS_BTDecorator_CheckAttackable, KeySelector));
	bNotifyBecomeRelevant = true;
	bNotifyDeactivation = true;
}

bool UNS_BTDecorator_CheckAttackable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                 uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	
	bool bIsAttackable = BlackboardComponent->GetValueAsBool(BlackboardKey.SelectedKeyName);
	return bIsAttackable;
}

void UNS_BTDecorator_CheckAttackable::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UNS_BTDecorator_CheckAttackable::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}
