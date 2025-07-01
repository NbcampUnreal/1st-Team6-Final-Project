// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Animations/Notify/NS_AnimNotify_ZombieEAttack.h"
#include "Zombie/NS_ZombieBase.h"
#include "Components/SphereComponent.h"

void UNS_AnimNotify_ZombieEAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (!Zombie) return;

	Zombie->GetR_SphereComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zombie->GetR_SphereComponent()->SetGenerateOverlapEvents(true);
}
