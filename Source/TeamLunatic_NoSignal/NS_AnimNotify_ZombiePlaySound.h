// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AnimNotify_ZombiePlaySound.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AnimNotify_ZombiePlaySound : public UAnimNotify
{
	GENERATED_BODY()
public:
	UNS_AnimNotify_ZombiePlaySound();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* SoundCue;
};
