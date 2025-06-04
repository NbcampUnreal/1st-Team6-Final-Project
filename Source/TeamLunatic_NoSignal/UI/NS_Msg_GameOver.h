// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_Msg_GameOver.generated.h"

class UButton;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_Msg_GameOver : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
	virtual void ShowWidgetD() override;

	UFUNCTION(BlueprintCallable)
	void PlayOpenAnimation();

	UFUNCTION()
	void OnLoadSavedGameBtnClicked();

	UFUNCTION()
	void OnMainMenuBtnClicked();

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* LoadSavedGameBtn;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* MainMenuBtn;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
};
