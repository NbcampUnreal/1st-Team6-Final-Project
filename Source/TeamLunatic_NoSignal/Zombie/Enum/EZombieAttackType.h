// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EZombieAttackType : uint8
{
	BASIC UMETA(DisplayName = "BASIC"),
	CHARGE UMETA(DisplayName = "CHARGE"),
	JUMP UMETA(DisplayName = "JUMP"),
};
