// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_BaseWidgetInterface.h"
#include "NS_SinglePlayWidget.generated.h"

class UButton;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SinglePlayWidget : public UUserWidget, public INS_BaseWidgetInterface
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

protected:

   UFUNCTION()
   void HandleButtonClicked(EWidgetToggleType ToggleType);

   UFUNCTION()
   void OnNewGameClicked();
   UFUNCTION()
   void OnLoadGameClicked();
   UFUNCTION()
   void OnMainMenuClicked();

    //// 각각 블루프린트에서 이름 매칭
    UPROPERTY(meta = (BindWidget))
    UButton* BP_WidgetToggleButton_NewGame;

    UPROPERTY(meta = (BindWidget))
    UButton* BP_WidgetToggleButton_LoadGame;

    UPROPERTY(meta = (BindWidget))
    UButton* BP_WidgetToggleButton_MainMenu;

};
