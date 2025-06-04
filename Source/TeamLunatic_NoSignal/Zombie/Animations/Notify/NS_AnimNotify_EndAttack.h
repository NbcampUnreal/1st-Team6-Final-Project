// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AnimNotify_EndAttack.generated.h"

class UBlackboardComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AnimNotify_EndAttack : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	FTimerHandle Timer;
	void SetAttackTimer(UBlackboardComponent* BlackboardComponent);
};
