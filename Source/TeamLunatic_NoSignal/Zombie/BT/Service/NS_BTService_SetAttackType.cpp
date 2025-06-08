// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_BTService_SetAttackType.h"

#include "NiagaraStatelessDefinitions.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/AIController/NS_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/Enum/EZombieAttackType.h"

UNS_BTService_SetAttackType::UNS_BTService_SetAttackType() : NewAttackType(EZombieAttackType::CHARGE)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
	Interval = 0.2f;
}

void UNS_BTService_SetAttackType::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ANS_AIController* AIController = Cast<ANS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return;
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(AIController->GetPawn());
	if (!Zombie) return;
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent) return;

	float Distance = BlackboardComponent->GetValueAsFloat("Distance");
	if (Distance <= 150.f)
	{
		BlackboardComponent->SetValueAsEnum("AttackType", static_cast<uint8>(EZombieAttackType::BASIC));
	}
	else if (Distance < 500.f)
	{
		BlackboardComponent->SetValueAsEnum("AttackType", static_cast<uint8>(EZombieAttackType::CHARGE));
	}
}
