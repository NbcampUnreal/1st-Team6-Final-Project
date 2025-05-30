// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NS_Service_CalculateDistance.generated.h"

enum class EZombieAttackType : uint8;
/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Service_CalculateDistance : public UBTService
{
	GENERATED_BODY()
	UNS_Service_CalculateDistance();
protected:
	UPROPERTY(EditAnywhere)
	float Distance;
	EZombieAttackType NewAttackType;
public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
