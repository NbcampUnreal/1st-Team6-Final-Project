// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NS_UIManager.generated.h"

class UNS_MainMenu;


UCLASS(Blueprintable)
class TEAMLUNATIC_NOSIGNAL_API UNS_UIManager : public UObject
{
	GENERATED_BODY()
	
public:
	UNS_UIManager();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitUi(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_MainMenu* GetNS_MainMenuWidget()const { return MainMenuWidget; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenuWidget(UWorld* World);
protected:
	UPROPERTY()
	UNS_MainMenu* MainMenuWidget;//

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_MainMenu> MainMenuWidgetClass;

private:

};
