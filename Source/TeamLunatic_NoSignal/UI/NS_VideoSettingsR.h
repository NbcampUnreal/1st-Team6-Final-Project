// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_VideoSettingsR.generated.h"

class UBorder;
class UTextBlock;
class UCheckBox;
class UButton;
class UNS_ResolutionSettingR;
class UNS_ScalerSettingR;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_VideoSettingsR : public UNS_MasterMenuPanel
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
	UButton* ApplyButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ResolutionSettingR* BP_ResolutionSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_WindowModeSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_VSyncSelector;

	//UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	//UNS_ScalerSettingR* BP_MaxFPSSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_AntiAliasingSelector;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_ViewDist;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_TextQuality;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_PostProcessing;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_Foliage;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_Effects;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UNS_ScalerSettingR* BP_ScalerSetting_Shadows;

};
