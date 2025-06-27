
#include "NS_StatueController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANS_StatueController::ANS_StatueController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.f;
	SightConfig->LoseSightRadius = 3500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ANS_StatueController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Blackboard && BehaviorTreeAsset)
	{
		Blackboard->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);
		RunBehaviorTree(BehaviorTreeAsset);

		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_StatueController::OnTargetPerceptionUpdated);
	}
}

void ANS_StatueController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	const bool bIsDetected = Stimulus.WasSuccessfullySensed();
	Blackboard->SetValueAsBool(FName(TEXT("bIsPlayerDetected")), bIsDetected);

	if (bIsDetected)
	{
		Blackboard->SetValueAsObject(FName(TEXT("TargetPlayer")), Actor);
		UE_LOG(LogTemp, Warning, TEXT("플레이어 감지!"));
	}
	else
	{
		Blackboard->SetValueAsObject(FName(TEXT("TargetPlayer")), nullptr);
		UE_LOG(LogTemp, Warning, TEXT("플레이어 감지 해제."));
	}
}