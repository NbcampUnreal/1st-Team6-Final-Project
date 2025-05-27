// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "Enum_ZombieType.generated.h"

UENUM(Blueprintable)
enum class Enum_ZombieType : uint8
{
	BASIC UMETA(DisplayName = "BASIC"),
	RUNNER UMETA(DisplayName = "RUNNER"),
	FAT UMETA(DisplayName = "FAT"),
};
