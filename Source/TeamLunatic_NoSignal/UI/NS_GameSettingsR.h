// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_GameSettingsR.generated.h"

class UBorder;
class UTextBlock;
class UCheckBox;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameSettingsR : public UNS_MasterMenuPanel
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
	UCheckBox* ShowItemTooltipsTickbox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UCheckBox* ShowHUDTickbox;
};
