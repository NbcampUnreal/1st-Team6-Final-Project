// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_InGameLoadMenu.h"
#include "NS_LoadMenuGameOver.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadMenuGameOver : public UNS_InGameLoadMenu
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void  LoadSaveSlotsToUI() override;
	virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
	virtual void ShowWidgetD() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UNS_MenuButtonWidget* MainMenuButton;

protected:
	UFUNCTION()
	void ReturnMainMenu();
};
