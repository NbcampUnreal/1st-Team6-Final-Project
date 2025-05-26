// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NS_CommonType.h"
#include "NS_BaseMainMenu.generated.h"

class UUserWidget;
class UNS_MasterMenuPanel;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_BaseMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	virtual void PlayAnimationShowR();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	virtual void PlayAnimationShowL();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	virtual UNS_MasterMenuPanel* GetWidget(EWidgetToggleType ToggleType);

	virtual void QuitNSGame();

protected:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	TMap<EWidgetToggleType, UNS_MasterMenuPanel*> WidgetMap;
};
