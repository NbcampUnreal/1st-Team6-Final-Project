// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTask_SetAttackType.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/Enum/EZombieAttackType.h"

EBTNodeResult::Type UNS_BTTask_SetAttackType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(AIController->GetPawn());
	if (Zombie == nullptr) return EBTNodeResult::Failed;
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent) return EBTNodeResult::Failed;

	float Distance = BlackboardComponent->GetValueAsFloat("Distance");
	if (Distance <= 250 || Distance > 250)
	{
		Zombie->SetAttackType(EZombieAttackType::CHARGE);
	}
	if (Distance <= 150)
	{
		Zombie->SetAttackType(EZombieAttackType::BASIC);
	}
	return EBTNodeResult::Succeeded;
}
