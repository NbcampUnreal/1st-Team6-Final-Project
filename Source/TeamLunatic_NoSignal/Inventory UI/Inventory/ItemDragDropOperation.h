// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.generated.h"

class UInventoryComponent;
class ANS_BaseItem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	ANS_BaseItem* SourceItem;

	UPROPERTY()
	UInventoryComponent* SourceInventory;
};
