// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Zombie/Enum/EZombieAttackType.h"
#include "NS_BTService_SetAttackType.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTService_SetAttackType : public UBTService
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	EZombieAttackType NewAttackType;
public:
	UNS_BTService_SetAttackType();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
