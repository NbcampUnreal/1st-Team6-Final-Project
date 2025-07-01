// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_BTTask_LookPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/AIController/NS_AIController.h"


UNS_BTTask_LookPlayer::UNS_BTTask_LookPlayer() : TargetRotation(0.f)
{
	bNotifyTick = true;
}



EBTNodeResult::Type UNS_BTTask_LookPlayer::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	Super::ExecuteTask(Comp, NodeMemory);
	ANS_AIController* AIController = Cast<ANS_AIController>(Comp.GetAIOwner());
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn||!AIController) return EBTNodeResult::Failed;


	return EBTNodeResult::InProgress;
}

void UNS_BTTask_LookPlayer::TickTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(Comp, NodeMemory, DeltaSeconds);
	ANS_AIController* AIController = Cast<ANS_AIController>(Comp.GetAIOwner());
	APawn* Pawn = AIController->GetPawn();
	AActor* Target = Cast<AActor>(Comp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	FVector Direction = Target->GetActorLocation() - Pawn->GetActorLocation();
	Direction.Z = 0;
	Direction.Normalize();
	
	TargetRotation = Direction.Rotation();
	
	
	if (!AIController||!Pawn) return;

	FRotator CurrentRotation = Pawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, 2.f);

	Pawn->SetActorRotation(NewRotation);
	Comp.GetBlackboardComponent()->SetValueAsFloat("PlayerRotation",NewRotation.Yaw-TargetRotation.Yaw);
	float DiffYaw = FMath::Abs(FRotator::NormalizeAxis(Pawn->GetActorRotation().Yaw - TargetRotation.Yaw));
	if (DiffYaw < 5.f)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool("bIsDetecting",true);
		FinishLatentTask(Comp, EBTNodeResult::Succeeded);
	}
}

uint16 UNS_BTTask_LookPlayer::GetInstanceMemorySize() const
{
	return Super::GetInstanceMemorySize();
}
