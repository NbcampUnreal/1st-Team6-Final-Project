// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTask_Attack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/Enum/EZombieAttackType.h"


UNS_BTTask_Attack::UNS_BTTask_Attack()
{
}

// EBTNodeResult::Type UNS_BTTask_Attack::ExecuteTask(UBehaviorTreeComponent& Comp)
// {
// 	APawn* ControlledPawn = Comp.GetAIOwner()->GetPawn();
// 	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(ControlledPawn);
// 	return EBTNodeResult::Succeeded;
// }
