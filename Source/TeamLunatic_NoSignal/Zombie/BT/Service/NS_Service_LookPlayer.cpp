// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_Service_LookPlayer.h"
#include "Zombie/AIController/NS_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_Service_LookPlayer::UNS_Service_LookPlayer()
{
	NodeName = "LookPlayer";
	bNotifyBecomeRelevant =true;
}

void UNS_Service_LookPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComponent();

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(Key.SelectedKeyName, Target->GetActorLocation());
	}
}
