// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Components/EditableTextBox.h"
#include "NS_NewGameR.generated.h"

class UEditableTextBox;
class USaveGame;
class UNS_AreYouSureMenu;
class UButton;
class UComboBoxString;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_NewGameR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintCallable)
    void OnStartGameClicked();

    UFUNCTION()
    void OnYesSelected();
     
    UFUNCTION()
    void OnNoSelected();

    virtual void Init(UNS_MainMenu* NsMainMenu) override;

protected:
    virtual void NativeConstruct() override;


    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UComboBoxString* ComboBoxString_mapName;
    

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget),BlueprintReadWrite)
    UEditableTextBox* EditableTextBox_SaveName;

  //  UPROPERTY(meta = (BindWidget),BlueprintReadWrite) //EditAnywhere
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category= "PopUp")
    UNS_AreYouSureMenu* AreYouSureMenu;

private:

    FString GetSaveSlotName() const;
    void StartGame();
    void ShowConfirmationMenu();//덮허쓰기할지 선택하는 메세지창 
    void HideConfirmationMenu(); // 덮허쓰기 창 닫기/캔슬
    FString FormatDateTime_YMDHM(const FDateTime& InDateTime);

   

};
