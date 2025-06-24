// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_AnimNotify_ZombieLDAttack.h"

#include "Components/SphereComponent.h"
#include "Zombie/NS_ZombieBase.h"

void UNS_AnimNotify_ZombieLDAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(Owner);
	if (!Zombie) return;
	
	Zombie->GetL_SphereComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Zombie->GetL_SphereComponent()->SetGenerateOverlapEvents(false);
}
