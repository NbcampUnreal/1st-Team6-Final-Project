// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "NS_MainUiPlayerController.generated.h"

class UNS_MainMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainUiPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> NS_MainMenuWidgetClass;

    UPROPERTY()
    UNS_MainMenu* NS_MainMenuWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* NS_SGKCharacterInputs;
};
