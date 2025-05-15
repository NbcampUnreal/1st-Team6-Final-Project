// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_ControlsSettings.generated.h"

class UTextBlock;
class UBorder;
class UButton;
class USlider;
class UScrollBox;
class UVerticalBox;
class UNS_NumericalTextBox;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ControlsSettings : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)//, BlueprintReadOnly / BlueprintReadWrite
    UTextBlock* Subtitle;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USlider* Slider_MouseSensitivity;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UScrollBox* InputsBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UVerticalBox* ControlVerticalBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UNS_NumericalTextBox* BP_NumericalTextBox;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UButton* DefaultsButton;

};
