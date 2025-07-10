// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_HostNewGameServerR.generated.h"

class UTextBlock;
class UTitle;
class UButton;
class UCheckBox;
class UEditableTextBox;
class USpacer;
class UBorder;
class UComboBoxString;
class UNS_AreYouSureMenu;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_HostNewGameServerR : public UUserWidget
{
	GENERATED_BODY()
	//슬랙에서 지우기 x
public:
	FString LoadMapName;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UEditableTextBox* SaveNameEntryBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USpacer* SelectMapSpacer_1;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UComboBoxString* ComboBoxString;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USpacer* SelectMapSpacer;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UButton* CreateServerButton;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnYesSelected();

    FString GetSaveSlotName() const;

    void StartGame();

	UFUNCTION()
    void OnCreateServerButtonClicked();
};
