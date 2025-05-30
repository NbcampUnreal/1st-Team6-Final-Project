#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NS_UIManager.generated.h"

class UNS_BaseMainMenu;

UCLASS(Blueprintable)
class TEAMLUNATIC_NOSIGNAL_API UNS_UIManager : public UObject
{
	GENERATED_BODY()
	
public:
	UNS_UIManager();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitUi(UWorld* World);

	bool IsInViewportInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_BaseMainMenu* GetNS_MainMenuWidget()const { return InGameMenuWidget; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInGameMenuWidget(UWorld* World);
	void HideInGameMenuWidget(UWorld* World);
protected:
	UPROPERTY()
	UNS_BaseMainMenu* InGameMenuWidget;//

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_BaseMainMenu> InGameMenuWidgetClass;

private:

};
