// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_LoadGameMenuPanel.generated.h"

class UTextBlock;
class UButton;
class UImage;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadGameMenuPanel : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* SaveNameText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite) 
	UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* DateText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* DeleteSaveButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* Image_DeleteIcon;

};
