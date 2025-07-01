// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_BTDecorator_CheckLookPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CheckLookPlayer::UNS_BTDecorator_CheckLookPlayer()
{
	NodeName = "CheckLookPlayer";
	bNotifyTick = true;
}

bool UNS_BTDecorator_CheckLookPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;
	
	return BlackboardComponent->GetValueAsBool(BlackboardKey.SelectedKeyName);
}
