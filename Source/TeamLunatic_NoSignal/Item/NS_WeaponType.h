#pragma once
#include "CoreMinimal.h"
#include "NS_WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Melee UMETA(DisplayName = "Melee"),
	Pistol UMETA(DisplayName = "Pistol"),
	Ranged UMETA(DisplayName = "Ranged"),
	Ammo UMETA(DisplayName = "Ammo"),
};