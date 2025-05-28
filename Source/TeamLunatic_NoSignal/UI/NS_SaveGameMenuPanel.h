// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_SaveGameMenuPanel.generated.h"

class UButton;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked1, UNS_SaveGameMenuPanel*, ClickedPanel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedDelete1, UNS_SaveGameMenuPanel*, ClickedPanel);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SaveGameMenuPanel : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
	public:
		virtual void NativeConstruct() override;

		UFUNCTION()
		void OnClickedDeleteSaveButton();
		UFUNCTION()
		void OnClickedSave();

		void SetSlotInfo(const FString& SlotName, const FString& LevelName, const FDateTime& SaveTime);

		UPROPERTY(BlueprintAssignable)
		FOnSlotClicked1 OnSlotClicked;

		UPROPERTY(BlueprintAssignable)
		FOnSlotClickedDelete1 OnDeleteSlotClicked;


		UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		UButton* SaveGameButton;

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
