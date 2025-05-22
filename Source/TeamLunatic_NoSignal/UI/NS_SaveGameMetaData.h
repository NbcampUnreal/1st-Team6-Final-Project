// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "NS_SaveGameMetaData.generated.h"

struct FSaveMetaData;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SaveGameMetaData : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FSaveMetaData> SaveMetaDataArray;
};
