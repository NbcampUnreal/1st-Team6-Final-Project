// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Zombie/NS_ZombieBase.h"
#include "NS_FatZombie.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_FatZombie : public ANS_ZombieBase
{
	GENERATED_BODY()
	ANS_FatZombie();

protected:
	virtual void OnStateChanged(EZombieState NewState) override;
};
