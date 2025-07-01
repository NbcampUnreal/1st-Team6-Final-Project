// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_HostLoadGameServerR.generated.h"

class UTextBlock;
class UTitle;
class UButton;
class UCheckBox;
class UEditableTextBox;
class USpacer;
class UBorder;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_HostLoadGameServerR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
    FString LoadSlotName;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite) //, BlueprintReadOnly / BlueprintReadWrite
        UTextBlock* Subtitle;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    USpacer* SelectMapSpacer;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UEditableTextBox* EditableTextBox_MaxPlayers;


    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UButton* CreateServerButton;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnCreateServerButtonClicked();
};
