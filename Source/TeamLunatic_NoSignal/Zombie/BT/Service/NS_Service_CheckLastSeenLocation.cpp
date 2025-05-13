// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_Service_CheckLastSeenLocation.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/AIController/NS_AIController.h"

UNS_Service_CheckLastSeenLocation::UNS_Service_CheckLastSeenLocation()
{
	NodeName = "CheckLastSeenLocation";
	Interval = 0.01f;
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UNS_Service_CheckLastSeenLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds){
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (UBlackboardComponent* BT = OwnerComp.GetBlackboardComponent())
	{
		if (const ANS_AIController* AIController = Cast<ANS_AIController>(OwnerComp.GetAIOwner()))
		{
			if (APawn* pawn = AIController->GetPawn())
			{
				FVector Location = BT->GetValueAsVector("LastSeenLocation");
				float Dist = FVector::Dist(pawn->GetActorLocation(), Location);
				if (Dist < AllowRange)
				{
					BT->ClearValue("LastSeenLocation");
				}
			}
		}
	}	
}
