#pragma once
#include "CoreMinimal.h"
#include "NS_WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Melee UMETA(DisplayName = "Knife"),
	Pistol UMETA(DisplayName = "P1191"),
	Ranged UMETA(DisplayName = "M4A4"),
	Ammo UMETA(DisplayName = "Ammo"),
};







