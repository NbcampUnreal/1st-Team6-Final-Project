// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTaskNode_ClearKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTTaskNode_ClearKey::UNS_BTTaskNode_ClearKey(){
	NodeName = "ClearKeyValue";
}
EBTNodeResult::Type UNS_BTTaskNode_ClearKey::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = Comp.GetBlackboardComponent();
	if(!BlackboardComponent) return EBTNodeResult::Failed;
	
	BlackboardComponent->ClearValue(KeySelector.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}