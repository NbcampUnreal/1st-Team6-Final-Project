// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "NS_EvnGen_SensedPlayer.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "SensedPlayer"))
class TEAMLUNATIC_NOSIGNAL_API UNS_EvnGen_SensedPlayer : public UEnvQueryGenerator
{
	GENERATED_BODY()

protected:
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

public:
	UNS_EvnGen_SensedPlayer();	
};
