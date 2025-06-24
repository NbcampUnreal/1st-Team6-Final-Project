// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Service/NS_Service_CalculateDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/Enum/EZombieAttackType.h"
#include "Zombie/AIController/NS_AIController.h"

UNS_Service_CalculateDistance::UNS_Service_CalculateDistance()
{
	NodeName = "Calculate Distance";
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
	Distance = 0.f;
}

void UNS_Service_CalculateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (APawn* Pawn = AIController->GetPawn())
		{
			if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
			{
				UObject* TargetObject = BlackboardComponent->GetValueAsObject("TargetActor");
				if (AActor* TargetActor = Cast<AActor>(TargetObject))
				{
					Distance = FVector::Dist(Pawn->GetActorLocation(), TargetActor->GetActorLocation());
					BlackboardComponent->SetValueAsFloat("Distance", Distance);
					float AttackRange = BlackboardComponent->GetValueAsFloat("AttackRange");
					if (AttackRange > Distance)
					{
						BlackboardComponent->SetValueAsBool("bIsAttackable", true);
					}
					else
					{
						BlackboardComponent->SetValueAsBool("bIsAttackable", false);
					}
					
					FVector DirectionToTarget = (TargetActor->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
					FVector ForwardVector = Pawn->GetActorForwardVector();

					float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
					float AngleDegrees = FMath::Acos(DotProduct) * (180.f / PI);
					
					float AcceptableAngle = 20.f;
					BlackboardComponent->SetValueAsBool("bIsLookAt", AngleDegrees <= AcceptableAngle);
				}
			}
		}
	}
}


