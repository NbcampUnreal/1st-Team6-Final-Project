// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTask_MoveTo.h"

EBTNodeResult::Type UNS_BTTask_MoveTo::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UNS_BTTask_MoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	
}
