// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"

UENUM(Blueprintable)
enum class Enum_ZombieState : uint8
{
	PATROLL UMETA(DisplayName = "PATROLL"),
	DETECTING UMETA(DisplayName = "DETECTING"),
	ATTACK UMETA(DisplayName = "ATTACK"),
	DEAD UMETA(DisplayName = "DEAD")
};
