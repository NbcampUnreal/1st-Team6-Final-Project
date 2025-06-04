// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NS_AIController.generated.h"

class UBehaviorTreeComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_AIController : public AAIController
{
	GENERATED_BODY()
	ANS_AIController();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UAIPerceptionComponent* Perception;
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsDetect;

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* PossessedPawn) override;
	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	void HandleSightStimulus();
	void HandleHearingStimulus(const FAIStimulus& Stimulus);
	void HandleDamageStimulus(AActor* Attacker);
	
	AActor* GetClosestSightTarget();

	void InitializeAttackRange(APawn* PossessedPawn);
	//Perception Configs
	void InitializingSightConfig();
	void InitializingHearingConfig();
};
