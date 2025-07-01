// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_InGameMsg.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameMsg : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void ShowMessageText(const FString& Message);

protected:
	void PlayFadeAnimation();

	UFUNCTION()
	void OnFadeAnimationFinished();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* TextMessage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FadeAnim;
	
	bool bCollapsed;
	FWidgetAnimationDynamicEvent OnFadeFinishedDelegate;
};
