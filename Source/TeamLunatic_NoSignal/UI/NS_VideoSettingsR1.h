// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_VideoSettingsR1.generated.h"

class UUserWidget;
class UBorder;
class UTextBlock;
class UCheckBox;
class UButton;
class UNS_ResolutionSettingR;
class UNS_ScalerSettingR;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_VideoSettingsR1 : public UNS_MasterMenuPanel
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

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* RefreshButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* ResolutionSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* WindowModeSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* VSyncSelector;


	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* AntiAliasingSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* ViewDist;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* Textures;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* PostProcessing;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* Foliage;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* Effects;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UUserWidget* Shadows;
};
