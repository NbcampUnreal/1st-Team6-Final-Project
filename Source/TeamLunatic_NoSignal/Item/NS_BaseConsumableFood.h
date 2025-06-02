#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseConsumable.h"
#include "NS_BaseConsumableFood.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseConsumableFood : public ANS_BaseConsumable
{
	GENERATED_BODY()

public:
	ANS_BaseConsumableFood();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	
	
};
