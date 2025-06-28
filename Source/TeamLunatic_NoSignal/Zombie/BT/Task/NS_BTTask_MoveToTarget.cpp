// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Task/NS_BTTask_MoveToTarget.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Zombie/AIController/NS_AIController.h"

UNS_BTTask_MoveToTarget::UNS_BTTask_MoveToTarget()
{
	NodeName = "MoveToTargetLocation";
}

EBTNodeResult::Type UNS_BTTask_MoveToTarget::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AICon || !Pawn || !BB || !BlackboardKey.IsSet())
	{
		return EBTNodeResult::Failed;
	}

	const FVector TargetLocation = BB->GetValueAsVector(BlackboardKey.SelectedKeyName);
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(TargetLocation);
	MoveReq.SetAcceptanceRadius(5.f);

	FNavPathSharedPtr NavPath;
	FPathFollowingRequestResult Result = AICon->MoveTo(MoveReq, &NavPath);

	if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		return EBTNodeResult::InProgress;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

void UNS_BTTask_MoveToTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if (TaskResult != EBTNodeResult::Succeeded) return;

	AAIController* Controller = OwnerComp.GetAIOwner();
	ANS_AIController* MyAI = Cast<ANS_AIController>(Controller);
	if (!MyAI || !BlackboardKey.IsSet()) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	APawn* Pawn = MyAI->GetPawn();
	if (!Pawn) return;

	const FVector CurrentLocation = Pawn->GetActorLocation();
	const FVector FinalTarget = MyAI->GetCurrentTargetLocation();
	const float Distance = FVector::Dist(CurrentLocation, FinalTarget);

	if (Distance < 200.f)
	{
		// 최종 도착: Clear
		BB->ClearValue(BlackboardKey.SelectedKeyName);
		
	}
	else
	{
		const FVector Direction = (FinalTarget - CurrentLocation).GetSafeNormal();
		const FVector NextStep = CurrentLocation + Direction * 200.f;
		BB->SetValueAsVector(BlackboardKey.SelectedKeyName, NextStep);
		if (TaskResult == EBTNodeResult::Succeeded && BlackboardKey.IsSet())
		{
			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);
			}
		}
	}
}
