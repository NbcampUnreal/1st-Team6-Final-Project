// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Zombie/Enum/EZombieState.h"
#include "AIController.h"
#include "NS_BTTask_SetZombieState.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTTask_SetZombieState : public UBTTaskNode
{
	GENERATED_BODY()
	UNS_BTTask_SetZombieState();
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Stat")
	EZombieState NewState;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
