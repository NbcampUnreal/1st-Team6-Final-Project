// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_BaseMainMenu.h"
#include "NS_InGameMenu.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameMenu : public UNS_BaseMainMenu
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ShowR;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ShowL;

	virtual void PlayAnimationShowR() override;
	virtual void PlayAnimationShowL() override;
	virtual void HideWidget() override;
	virtual void ShowWidget() override;
	//UNS_MasterMenuPanel* GetWidget(EWidgetToggleType ToggleType);

	//void QuitNSGame();
private:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	//TMap<EWidgetToggleType, UNS_MasterMenuPanel*> WidgetMap;
};
