// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EZombieType.generated.h"

UENUM(BlueprintType)
enum class EZombieType : uint8
{
	BASIC UMETA(DisplayName = "BASIC"), 
	RUNNER UMETA(DisplayName = "RUNNER"), 
	FAT UMETA(DisplayName = "FAT")
};
