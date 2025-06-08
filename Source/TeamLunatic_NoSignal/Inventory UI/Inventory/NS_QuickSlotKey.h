// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotKey.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotKey : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetSlotIndex(int32 Index);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QSText;
};
