#pragma once

#include "CoreMinimal.h"
#include "Item/NS_InventoryBaseItem.h"
#include "NS_BaseConsumableItem.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BaseConsumableItem : public UNS_InventoryBaseItem
{
	GENERATED_BODY()
	
public:
	UNS_BaseConsumableItem();

	virtual void OnUseItem() override;

	void ApplyConsumableEffect();

	void InitializeFromDataTable();

protected:

};
