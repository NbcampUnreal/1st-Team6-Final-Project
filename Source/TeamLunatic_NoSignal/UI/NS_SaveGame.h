// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UI/NS_CommonType.h"
#include "NS_SaveGame.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FPlayerSaveData> PlayerSaves;

    UPROPERTY(BlueprintReadWrite)
    TArray<FLevelSaveData> LevelSaves;
};
