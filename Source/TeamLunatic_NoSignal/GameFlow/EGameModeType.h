#pragma once

#include "CoreMinimal.h"
#include "EGameModeType.generated.h"

UENUM(BlueprintType)
enum class EGameModeType : uint8
{
    SinglePlayMode   UMETA(DisplayName = "SinglePlayMode"),
    MultiPlayMode    UMETA(DisplayName = "MultiPlayMode")
};
