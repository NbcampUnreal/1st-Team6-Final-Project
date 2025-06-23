// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Animations/Notify/NS_AnimNotify_ZombieDAttack.h"
#include "Components/SphereComponent.h"
#include "Zombie/NS_ZombieBase.h"


void UNS_AnimNotify_ZombieDAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (!Zombie) return;

	Zombie->GetR_SphereComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
