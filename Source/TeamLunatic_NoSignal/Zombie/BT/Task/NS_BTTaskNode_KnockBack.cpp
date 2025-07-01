// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTaskNode_KnockBack.h"

#include "Zombie/NS_ZombieBase.h"
#include "Zombie/AIController/NS_AIController.h"

UNS_BTTaskNode_KnockBack::UNS_BTTaskNode_KnockBack()
{
	NodeName = "KnockBack";
	bNotifyTick = true;
}

EBTNodeResult::Type UNS_BTTaskNode_KnockBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(OwnerComp.GetOwner());
	if (!Zombie) return EBTNodeResult::Failed;
	ANS_AIController* AICon = Cast<ANS_AIController>(Zombie->GetController());
	if (!AICon) return EBTNodeResult::Failed;

	Zombie->Multicast_PlayMontage_Implementation(MontageToPlay);

	return EBTNodeResult::InProgress;
}

void UNS_BTTaskNode_KnockBack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(OwnerComp.GetOwner());
	if (!Zombie || !MontageToPlay)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	UAnimInstance* AnimInstance = Zombie->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AnimInstance->Montage_IsPlaying(MontageToPlay))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
