// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_AudioSettingsR.generated.h"

class UBorder;
class UTextBlock;
class UCheckBox;
class UButton;
class USlider;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AudioSettingsR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UBorder* Backround;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)//, BlueprintReadOnly / BlueprintReadWrite
	UTextBlock* Subtitle;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* Title;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	USlider* MasterSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	USlider* EffectSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	USlider* AmbientSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	USlider* MusicSlider;

};
