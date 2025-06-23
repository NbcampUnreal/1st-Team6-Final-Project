// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/AIController/NS_AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Zombie/NS_ZombieBase.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/Enum/EZombieType.h"

ANS_AIController::ANS_AIController() : MaxSeenDistance(1000.f)
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	SetPerceptionComponent(*Perception);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConfig");
	InitializingSightConfig();
	InitializingHearingConfig();

	Perception -> ConfigureSense(*SightConfig);
	Perception -> ConfigureSense(*HearingConfig);
	
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
}

void ANS_AIController::BeginPlay()
{
	Super::BeginPlay();
	SetTargetPoint();
}

void ANS_AIController::SetTargetPoint()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetClass, TargetActors);
	if (TargetActors.Num() > 0)
	{
		for (AActor* Target : TargetActors)
		{
			TargetLocations.Push(Target->GetActorLocation());
		}
	}
	if (TargetLocations.Num() > 0)
	{
		int RandomIndex = FMath::RandRange(0, TargetActors.Num() - 1);
		FVector Location = TargetLocations[RandomIndex];
		GetBlackboardComponent()->SetValueAsVector("TargetLocation", Location);
	}
}

void ANS_AIController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(PossessedPawn);
	switch (Zombie->GetType())
	{
	case EZombieType::FAT:
	case EZombieType::BASIC:
		if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
		{
			RunBehaviorTree(BehaviorTreeAsset);
		}
		break;
	case EZombieType::RUNNER:
		if (UseBlackboard(RunnerBehaviorTreeAsset->BlackboardAsset, BlackboardComp))
		{
			RunBehaviorTree(RunnerBehaviorTreeAsset);
		}
		break;
	default:
		break;
	}
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_AIController::OnPerceptionUpdated);
	InitializeAttackRange(PossessedPawn);
}

void ANS_AIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	FString SenseType = Stimulus.Type.Name.ToString();
	if (SenseType == "Default__AISense_Sight")
	{
		HandleSightStimulus();
	}
	else if (SenseType == "Default__AISense_Hearing")
	{
		HandleHearingStimulus(Stimulus.StimulusLocation);
	}
	else if (SenseType == "Default__AISense_Damage")
	{
		HandleDamageStimulus(Actor);
	}
}

void ANS_AIController::PauseBT()
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic("PauseBT");
	}
}

void ANS_AIController::ResumeBT()
{
	if (BrainComponent)
	{
		BrainComponent->StartLogic();
	}
}

void ANS_AIController::HandleSightStimulus()
{
	AActor* Actor = GetClosestSightTarget();

	if (Actor)
	{
		LastSeenTarget = Actor;
		BlackboardComp->SetValueAsObject("TargetActor", Actor);
	}
	else if (LastSeenTarget)
	{
		// 거리 검사
		FVector AILocation = GetPawn()->GetActorLocation();
		float Distance = FVector::DistSquared(LastSeenTarget->GetActorLocation(), AILocation);

		if (Distance < FMath::Square(MaxSeenDistance))
		{
			BlackboardComp->SetValueAsObject("TargetActor", LastSeenTarget);
		}
		else
		{
			LastSeenTarget = nullptr;
			BlackboardComp->ClearValue("TargetActor");
		}
	}
	else
	{
		BlackboardComp->ClearValue("TargetActor");
	}
}

void ANS_AIController::HandleHearingStimulus(FVector Location)
{
	const FVector NewHeardLocation = Location;
	FVector CurrentLocation = Blackboard->GetValueAsVector("HeardLocation");

	if (FVector::Dist(NewHeardLocation, CurrentLocation) > 500.f)
	{
		Blackboard->SetValueAsVector("HeardLocation", NewHeardLocation);
		GetWorldTimerManager().SetTimer(HearingTimerHandle,this, &ANS_AIController::InitializeHeardBool, 5.f, false);
		Blackboard->SetValueAsBool("bWasAlreadyHeard", true);
		CalculateHeardRotation(NewHeardLocation);
	}
}

void ANS_AIController::InitializeHeardBool()
{
	BlackboardComp->SetValueAsBool("bWasAlreadyHeard", false);
	GetWorldTimerManager().ClearTimer(HearingTimerHandle);
}

void ANS_AIController::HandleDamageStimulus(AActor* Attacker)
{
	BlackboardComp->SetValueAsObject("TargetActor", Attacker);
}

void ANS_AIController::CalculateHeardRotation(FVector HeardLocation)
{
	APawn* Zombie= GetPawn();
	if (!Zombie) return;
	FVector ZombieLocation = Zombie->GetActorLocation();
	FVector Direction = (HeardLocation - ZombieLocation).GetSafeNormal();

	FRotator TargetRotator = Direction.Rotation();

	FRotator ZombieRotator = Zombie->GetActorRotation();

	float Yaw = FMath::FindDeltaAngleDegrees(ZombieRotator.Yaw,TargetRotator.Yaw);
	BlackboardComp->SetValueAsFloat("DeltaYaw", Yaw);
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

void ANS_AIController::SetDisableAttackTimer()
{
	BlackboardComp->SetValueAsBool("bCanAttackAgain", false);
}

void ANS_AIController::SetEnableAttackTimer()
{
	BlackboardComp->SetValueAsBool("bCanAttackAgain", true);
}

void ANS_AIController::InitializeAttackRange(APawn* PossesedPawn)
{
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(PossesedPawn);
	if (!Zombie) return;
	EZombieType Type = Zombie->GetType();
	AttackRange = 0.f;
	switch (Type)
	{
	case EZombieType::BASIC:
		AttackRange=300.f;
		BlackboardComp->SetValueAsFloat("AttackRange", AttackRange);
		break;
	case EZombieType::RUNNER:
		AttackRange=300.f;
		BlackboardComp->SetValueAsFloat("AttackRange", AttackRange);
		break;
	case EZombieType::FAT:
		AttackRange=200.f;
		BlackboardComp->SetValueAsFloat("AttackRange", AttackRange);
		break;
	default:
		break;
	}
}

void ANS_AIController::InitializingSightConfig()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 600.f;
		SightConfig->LoseSightRadius = 150.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(12.f);
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
		HearingConfig->HearingRange = 3000.f;
		HearingConfig->SetMaxAge(0.f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	}
}