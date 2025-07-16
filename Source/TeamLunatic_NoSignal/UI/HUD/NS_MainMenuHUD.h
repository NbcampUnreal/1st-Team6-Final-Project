#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NS_MainMenuHUD.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainMenuHUD : public AHUD
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> SinglePlayWidgetclass;
	
	//UI
	UPROPERTY()
	UUserWidget* MainMenuWidget = nullptr;

	UPROPERTY()
	UUserWidget* SinglePlayWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ShowMainMenuUI();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void HideMainMenuUI();
	
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ShowSingleUI();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void HideSingleUI();
};
