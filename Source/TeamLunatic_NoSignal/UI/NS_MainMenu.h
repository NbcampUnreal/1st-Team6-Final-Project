// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_BaseMainMenu.h"
#include "NS_MainMenu.generated.h"

class UUserWidget;
class UNS_MasterMenuPanel;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MainMenu : public UNS_BaseMainMenu//UUserWidget
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
};
