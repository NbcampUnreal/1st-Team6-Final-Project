// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_AnimNotify_ZombiePlaySound.h"
#include "ZOmbie/NS_ZombieBase.h"

UNS_AnimNotify_ZombiePlaySound::UNS_AnimNotify_ZombiePlaySound() : SoundCue(nullptr) {}

void UNS_AnimNotify_ZombiePlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (Zombie)
	{
		Zombie->Server_PlaySound(SoundCue);
	}
}
