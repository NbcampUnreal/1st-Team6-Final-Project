// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/AIController/NS_AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ANS_AIController::ANS_AIController()
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	SetPerceptionComponent(*Perception);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConfig");
	InitializingSightConfig();
	InitializingHearingConfig();

	Perception-> ConfigureSense(*SightConfig);
	Perception-> ConfigureSense(*HearingConfig);
	
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
}

void ANS_AIController::BeginPlay()
{
	Super::BeginPlay();
	if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}

	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_AIController::OnPerceptionUpdated);
}

void ANS_AIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	FString SenseName = Stimulus.Type.Name.ToString();
	if (SenseName == "Default__AISense_Sight")
	{
		HandleSightStimulus();
	}
	else if (SenseName == "Default__AISense_Hearing")
	{
		HandleHearingStimulus(Stimulus);
	}
	else if (SenseName == "Default__AISense_Damage"){
		HandleDamageStimulus(Actor);
	}
}

void ANS_AIController::HandleSightStimulus()
{
	AActor* Actor = GetClosestSightTarget();
	if (Actor)
	{
		BlackboardComp->SetValueAsObject("TargetActor", Actor);
	}
	else
	{
		BlackboardComp->ClearValue("TargetActor");
	}
}

void ANS_AIController::HandleHearingStimulus(const FAIStimulus& Stimulus)
{	
	BlackboardComp->SetValueAsVector("HeardLocation", Stimulus.StimulusLocation);	
}

void ANS_AIController::HandleDamageStimulus(AActor* Attacker)
{
	BlackboardComp->SetValueAsObject("TargetActor", Attacker);
}


AActor* ANS_AIController::GetClosestSightTarget()
{
	TArray<AActor*> Targets;
	float ClosestDist = FLT_MAX;
	Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Targets);
	FVector PawnLocation = GetPawn()->GetActorLocation();
	
	AActor* ClosestSightTarget = nullptr;
	
	for (AActor* Actor : Targets)
	{
		if (!Actor->ActorHasTag("Player")) continue;

		float dist = FVector::DistSquared(Actor->GetActorLocation(), PawnLocation);
		if (dist < ClosestDist)
		{
			ClosestDist = dist;
			ClosestSightTarget = Actor;
		}
	}
	if (ClosestSightTarget) return ClosestSightTarget;
	return nullptr;
}

void ANS_AIController::InitializingSightConfig()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 1500.f;
		SightConfig->LoseSightRadius = 1800.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(1.f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 100.f;
	}
}

void ANS_AIController::InitializingHearingConfig()
{
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 1200.f;
		HearingConfig->SetMaxAge(1.f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	}
}