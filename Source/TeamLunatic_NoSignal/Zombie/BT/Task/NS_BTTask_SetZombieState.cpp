// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTask_SetZombieState.h"

#include "MaterialHLSLTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/NS_ZombieBase.h"

UNS_BTTask_SetZombieState::UNS_BTTask_SetZombieState() : NewState(EZombieState::PATROLL)
{
}

EBTNodeResult::Type UNS_BTTask_SetZombieState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(AIController->GetPawn());
	if (Zombie == nullptr) return EBTNodeResult::Failed;
	
	EZombieAttackType AttackType = static_cast<EZombieAttackType>(AIController->GetBlackboardComponent()->GetValueAsEnum("AttackType"));
	Zombie->SetAttackType(AttackType);
	Zombie->SetState(NewState);
	return EBTNodeResult::Succeeded;
}
