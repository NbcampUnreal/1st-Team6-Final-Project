// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NS_BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	UNS_BTTask_Attack();
public:
	// EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp);
};
