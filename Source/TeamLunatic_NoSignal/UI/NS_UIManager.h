#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Ticker.h"  // 
#include "NS_UIManager.generated.h"

class UNS_BaseMainMenu;
class UNS_Msg_GameOver;
class UNS_InGameMsg;
class UNS_PlayerHUD;
class UNS_CircleProgressBar;
class UNS_InGameMenu;
class UNS_QuickSlotPanel;
class UNS_LoadingScreen;

DECLARE_DELEGATE(FOnLoadingFinished);

UCLASS(Blueprintable)
class TEAMLUNATIC_NOSIGNAL_API UNS_UIManager : public UObject
{
	GENERATED_BODY()
	
public:
	UNS_UIManager();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitUi(UWorld* World);

	UNS_QuickSlotPanel* GetQuickSlotPanel();

	bool IsInViewportInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_InGameMenu* GetNS_MainMenuWidget()const { return InGameMenuWidget; }
	UNS_PlayerHUD* GetPlayerHUDWidget() const { return NS_PlayerHUDWidget; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool ShowInGameMenuWidget(UWorld* World);
	void ShowLoadGameWidget(UWorld* World);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInGameMenuWidget(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool ShowGameOverWidget(UWorld* World);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameOverWidget(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool ShowGameMsgWidget(FString& GameMsg, UWorld* World);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameMsgWidget(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool ShowPlayerHUDWidget( UWorld* World);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePlayerHUDWidget(UWorld* World);

	void SetFInputModeGameAndUI(APlayerController* PC, UUserWidget* Widget);
	void SetFInputModeGameOnly(APlayerController* PC);

	void LoadingScreen(UWorld* World);
	void CloseLoadingUI();

	FOnLoadingFinished OnLoadingFinished;

protected:
	UPROPERTY()
	UNS_InGameMenu* InGameMenuWidget;
	UNS_Msg_GameOver* NS_Msg_GameOveWidget;
	UNS_InGameMsg* NS_InGameMsgWidget;
	UNS_PlayerHUD* NS_PlayerHUDWidget;
	UNS_LoadingScreen* NS_LoadingScreen;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_BaseMainMenu> InGameMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_Msg_GameOver> NS_MsgGameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_InGameMsg> NS_InGameMsgWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_PlayerHUD> NS_PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_LoadingScreen> NS_LoadingScreenClass;
	
private:

};
