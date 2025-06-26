// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "NS_EndingStatusUI.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_EndingStatusUI : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Ending UI")
	void EndingUpdateStatus(int32 NumPlayersInZone, int32 TotalPlayerCount, int32 NumItemsCollected);

	UFUNCTION(BlueprintCallable, Category = "Ending UI")
	void UpdateRemainingTime(float TimeRemaining);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerText;
};
