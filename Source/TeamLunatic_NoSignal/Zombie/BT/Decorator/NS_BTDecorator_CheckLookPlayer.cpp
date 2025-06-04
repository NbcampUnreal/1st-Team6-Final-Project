// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/Decorator/NS_BTDecorator_CheckLookPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UNS_BTDecorator_CheckLookPlayer::UNS_BTDecorator_CheckLookPlayer()
{
	NodeName = TEXT("CheckLookPlayer");
	AcceptableViewAngle = 10.f;
}

bool UNS_BTDecorator_CheckLookPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                 uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!AIPawn)
		return false;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(KeySelector.SelectedKeyName));

	if (!TargetActor)
		return false;

	FVector DirectionToTarget = (TargetActor->GetActorLocation() - AIPawn->GetActorLocation()).GetSafeNormal();
	FVector ForwardVector = AIPawn->GetActorForwardVector();

	float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
	float AngleDegrees = FMath::Acos(DotProduct) * (180.f / PI);

	return AngleDegrees <= AcceptableViewAngle;

}
