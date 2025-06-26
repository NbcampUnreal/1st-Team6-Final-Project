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


};
