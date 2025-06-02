// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Zombie/NS_ZombieBase.h"
#include "NS_RunnerZombie.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_RunnerZombie : public ANS_ZombieBase
{
	GENERATED_BODY()

public:
	ANS_RunnerZombie();
	virtual void OnStateChanged(EZombieState NewState) override;
};
