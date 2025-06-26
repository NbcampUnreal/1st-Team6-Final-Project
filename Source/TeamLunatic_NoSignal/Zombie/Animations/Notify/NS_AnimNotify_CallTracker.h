// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AnimNotify_CallTracker.generated.h"

class ANS_ChaserController;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AnimNotify_CallTracker : public UAnimNotify
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere,	BlueprintReadOnly, Category = "Actor")
	TSubclassOf<AActor> TrackedActorBP;

public:
	UNS_AnimNotify_CallTracker();
	float MaxHeardRange;
	float Loudness;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundCue* ScreamSound;
};
