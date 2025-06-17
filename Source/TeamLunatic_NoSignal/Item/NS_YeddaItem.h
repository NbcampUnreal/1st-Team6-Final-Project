// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_YeddaItem.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_YeddaItem : public ANS_BaseItem
{
	GENERATED_BODY()
public:
	ANS_YeddaItem();
	virtual void BeginPlay() override;
	void TryAssignToHUD();
};
