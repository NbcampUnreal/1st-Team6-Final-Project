// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_BaseWidgetInterface.h"
#include "NS_LogoScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LogoScreenWidget : public UUserWidget, public INS_BaseWidgetInterface
{
	GENERATED_BODY()
public:
	//UPROPERTY(meta = (BindWidgetAnim), Transient)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShowLogo;
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnShowLogoFinished();
};
