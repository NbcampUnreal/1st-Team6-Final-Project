#pragma once

#include "CoreMinimal.h"
#include "NS_EAmmoType.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	UAM_9mm UMETA(DisplayName = "9mm"),
	UAM_566mm UMETA(DisplayName = "5.66m")
};