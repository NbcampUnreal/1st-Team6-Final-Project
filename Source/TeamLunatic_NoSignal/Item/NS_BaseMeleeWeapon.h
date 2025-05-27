// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_BaseMeleeWeapon.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseMeleeWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()
	
public:
	ANS_BaseMeleeWeapon();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Effects")
	UParticleSystem* ImpactParticles;

public:
};
