// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"

UENUM(Blueprintable)
enum class Enum_ZombieState : uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	PATROLL UMETA(DisplayName = "PATROLL"),
	DETECTING UMETA(DisplayName = "DETECTING"),
	CHACING UMETA(DisplayName = "CHACING"),
	ATTACK UMETA(DisplayName = "ATTACK"),
	PUSHED UMETA(DisplayName = "PUSHED"),
	DEAD UMETA(DisplayName = "DEAD")
};
