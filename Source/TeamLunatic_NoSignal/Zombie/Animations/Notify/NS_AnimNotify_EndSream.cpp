// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Animations/Notify/NS_AnimNotify_EndSream.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/AIController/NS_AIController.h"
#include "Zombie/NS_ZombieBase.h"
UNS_AnimNotify_EndSream::UNS_AnimNotify_EndSream()
{
}

void UNS_AnimNotify_EndSream::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (!Zombie) return;
	ANS_AIController* Controller = Cast<ANS_AIController>(Zombie->GetController());
	if (!Controller) return;
	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	if (!BlackboardComponent) return;
	
	Controller->GetWorldTimerManager().SetTimer(Timer,Controller,&ANS_AIController::SetEnableAttackTimer,10.f,false); 
	BlackboardComponent->SetValueAsBool("bIsAttacking", false);
}
