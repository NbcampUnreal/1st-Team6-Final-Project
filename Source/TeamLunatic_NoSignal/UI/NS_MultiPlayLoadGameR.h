// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_MultiPlayLoadGameR.generated.h"

class UTextBlock;
class UImage;
class UScrollBox;
class UVerticalBox;
class UCheckBox;
class UNS_MenuButtonWidget;
class UNS_LoadGameMenuPanel;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MultiPlayLoadGameR : public UNS_MasterMenuPanel
{
    GENERATED_BODY()

public:
    UNS_MultiPlayLoadGameR(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "Load UI")
    void LoadSaveSlotsToUI();

   

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Load UI")
    TSubclassOf<UNS_LoadGameMenuPanel> LoadGameDataElementClass;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite) //, BlueprintReadOnly / BlueprintReadWrite
        UTextBlock* Subtitle;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UScrollBox* InputsBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UVerticalBox* SaveVerticalBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UNS_MenuButtonWidget* NewGameButton;

protected:
    UNS_LoadGameMenuPanel* SelectChildPanel;

    virtual void NativeConstruct() override;
	virtual void Init(UNS_MainMenu* NsMainMenu) override;
   
    UFUNCTION()
    void NewGame();

    UFUNCTION()
    void OnClickedDeleteSlot(UNS_LoadGameMenuPanel* ChidPanel);

    UFUNCTION()
    void OnClickedSelectChildPanel(UNS_LoadGameMenuPanel* ChidPanel);

private:
  
};
