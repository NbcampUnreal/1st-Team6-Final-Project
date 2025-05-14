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

	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_AIController::OnTargetDetected);
}

void ANS_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
		UE_LOG(LogTemp, Error, TEXT("BehaviorTreeAsset is!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BehaviorTreeAsset is null!"));
	}
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

void ANS_AIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Actor->ActorHasTag(TEXT("Player")))
		{
			GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
		}
	}
	else
	{
		GetBlackboardComponent()->ClearValue("TargetActor");
		BlackboardComp->SetValueAsVector("LastSeenLocation", Stimulus.StimulusLocation);
	}
}