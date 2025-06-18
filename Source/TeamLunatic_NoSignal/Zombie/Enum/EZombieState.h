// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	PATROLL UMETA(DisplayName = "PATROLL"),
	DETECTING UMETA(DisplayName = "DETECTING"),
	HEARTING UMETA(DisplayName = "HEARTING"),
	CHACING UMETA(DisplayName = "CHACING"),
	ATTACK UMETA(DisplayName = "ATTACK"),
	PUSHED UMETA(DisplayName = "PUSHED"),
	FROZEN UMETA(DisplayName = "FROZEN"),
	DEAD UMETA(DisplayName = "DEAD"),
};
