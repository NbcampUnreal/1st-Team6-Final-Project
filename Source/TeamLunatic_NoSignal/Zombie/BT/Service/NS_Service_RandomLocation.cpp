// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_Service_RandomLocation.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UNS_Service_RandomLocation::UNS_Service_RandomLocation()
{
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
	Interval = 0.5f;
	Radius = 500.f;
	NodeName = "Generate Random Location";
}

void UNS_Service_RandomLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!AIPawn) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FVector Origin = AIPawn->GetActorLocation();
	FNavLocation RandomLocation;

	if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(RandomLocationKey.SelectedKeyName,RandomLocation.Location);
	}
}



