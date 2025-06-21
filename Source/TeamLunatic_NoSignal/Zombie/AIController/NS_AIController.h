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
class UBrainComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_AIController : public AAIController
{
	GENERATED_BODY()

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* RunnerBehaviorTreeAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UAnimMontage* DetectMontage;
	UPROPERTY()
	AActor* LastSeenTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange;
	
	//스폰 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	TArray<FVector> TargetLocations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	TArray<AActor*> TargetActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Target")
	TSubclassOf<AActor> TargetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	FVector TargetLocation;
	
	FTimerHandle HearingTimerHandle;
	
	float MaxSeenDistance;
public:
	ANS_AIController();
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* PossessedPawn) override;
	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void PauseBT();
	void ResumeBT();
	
	void HandleSightStimulus();
	void HandleHearingStimulus(FVector Location);
	void HandleDamageStimulus(AActor* Attacker);
	void CalculateHeardRotation(FVector HeardLocation);
	void InitializeHeardBool();
	//스폰 함수
	void SetTargetPoint();
	
	
	AActor* GetClosestSightTarget();
	UFUNCTION()
	void SetDisableAttackTimer();
	UFUNCTION()
	void SetEnableAttackTimer();
	
	void InitializeAttackRange(APawn* PossessedPawn);
	//Perception Configs
	void InitializingSightConfig();
	void InitializingHearingConfig();

	FORCEINLINE FVector GetTargetLocation() const { return TargetLocation; }
};


