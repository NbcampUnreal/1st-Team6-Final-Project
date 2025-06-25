// NS_StatueController.cpp

#include "NS_StatueController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h" // BrainComponent를 위해 추가
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANS_StatueController::ANS_StatueController()
{
	// 1. 컴포넌트를 생성하여 '상속받은' 변수에 할당합니다.
	//    헤더에서 선언을 지웠지만, 부모 클래스(AAIController)에 변수가 존재하므로 사용 가능합니다.
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	// 2. AIPerceptionComponent 생성 및 설정 (이 부분은 동일)
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.f;
	SightConfig->LoseSightRadius = 3500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ANS_StatueController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 이 함수는 블랙보드가 유효한지 먼저 확인하는 것이 더 안전합니다.
	if (Blackboard && BehaviorTreeAsset)
	{
		Blackboard->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);

		// 비헤이비어 트리 실행은 BrainComponent를 통해 이루어집니다.
		// RunBehaviorTree 함수가 내부적으로 BrainComponent를 사용합니다.
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