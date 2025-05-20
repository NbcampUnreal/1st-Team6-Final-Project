// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_LoadGameMenuPanel.generated.h"

class UTextBlock;
class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, UNS_LoadGameMenuPanel*, ClickedPanel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedDelete, UNS_LoadGameMenuPanel*, ClickedPanel);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadGameMenuPanel : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	//ex) "SaveSlot_01" / Map : "Map_01"  => SaveSlotName = SaveSlot_01
	FString SaveSlotName; // 문자열뒤에 Map(Level)명은 빠진거임. 일단 이것으로 비교검색할것! 
	
	
	UPROPERTY(BlueprintAssignable)
	FOnSlotClicked OnSlotClicked;

	UPROPERTY(BlueprintAssignable)
	FOnSlotClickedDelete OnDeleteSlotClicked;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UImage* Image_Select;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UButton* LoadSaveButton;

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

	void SetSlotInfo(const FString& SlotName, const FString& LevelName, const FDateTime& SaveTime);

	UFUNCTION()
	void OnDeleteSaveButtonClicked();

	UFUNCTION()
	void OnClickedSelectPanel();

	void SelectedCheck();
	void UnSelectedCheck();
};
