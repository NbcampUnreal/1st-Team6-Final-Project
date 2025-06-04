// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Animations/Notify/NS_AnimNotify_EndAttack.h"
#include "Zombie/AIController/NS_AIController.h"
#include "Zombie/NS_ZombieBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"


void UNS_AnimNotify_EndAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (!Zombie) return;
	ANS_AIController* Controller = Cast<ANS_AIController>(Zombie->GetController());
	if (!Controller) return;
	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	if (!BlackboardComponent) return;
}
