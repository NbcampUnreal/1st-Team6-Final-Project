#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NS_Service_CheckPlayerVisibility.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Service_CheckPlayerVisibility : public UBTService
{
	GENERATED_BODY()

public:
	UNS_Service_CheckPlayerVisibility();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerLookingKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPlayerKey;
};