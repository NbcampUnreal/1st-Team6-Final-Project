// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_InGameLoadMenu.generated.h"

class UTextBlock;
class UImage;
class UScrollBox;
class UVerticalBox;
class UCheckBox;
class UNS_MenuButtonWidget;
class UNS_LoadGameMenuPanel;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameLoadMenu : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
public:
    UNS_InGameLoadMenu(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    virtual void ShowWidgetD() override;

    UFUNCTION(BlueprintCallable, Category = "Load UI")
    void LoadSaveSlotsToUI();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Load UI")
    TSubclassOf<UNS_LoadGameMenuPanel> LoadGameDataElementClass;

    //UPROPERTY(meta = (BindWidget), BlueprintReadWrite) //, BlueprintReadOnly / BlueprintReadWrite
    //    UTextBlock* Subtitle;
    //UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    //UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UScrollBox* InputsBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UVerticalBox* SaveVerticalBox;

    //UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    //UNS_LoadGameMenuPanel* BP_LoadGameMenuPanel;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UNS_MenuButtonWidget* StartGameButton;

protected:
    UNS_LoadGameMenuPanel* SelectChildPanel;

    UFUNCTION()
    void StartGame();

    UFUNCTION()
    void OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChidPanel);

    UFUNCTION()
    void OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChidPanel);
};
