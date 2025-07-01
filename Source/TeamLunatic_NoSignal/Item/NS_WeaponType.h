#pragma once
#include "CoreMinimal.h"
#include "NS_WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Melee UMETA(DisplayName = "Knife"),
	Pistol UMETA(DisplayName = "P1911"),
	Ranged UMETA(DisplayName = "M4A4"),
	ThrowActor UMETA(DisplayName = "Bottle"),
	Ammo UMETA(DisplayName = "Ammo"),
	Magazine UMETA(DisplayName = "Magazine"),
	BaseBallBat UMETA(DisplayName = "BaseBallBat")
};

UENUM(BlueprintType)
enum class ERangeChangeFireMode : uint8
{
	Manual UMETA(DisplayName = "Manual"),
	Auto UMETA(DisplayName = "Auto"),
};








