// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_InGameLoadMenu.h"
#include "NS_LoadMenuGameOver.generated.h"


UCLASS() //
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadMenuGameOver : public UNS_InGameLoadMenu
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void ShowWidgetD() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UNS_MenuButtonWidget* MainMenuButton;

	/*
	UNS_InGameLoadMenu를 상속받고 있어서 부모클래스에 아래 변수,함수가 설정되어 있다. 해매지 말것!

		UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		class UNS_MenuButtonWidget* StartGameButton;

		UFUNCTION()
		void StartGame();
	*/
protected:
	virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
	virtual void  LoadSaveSlotsToUI() override;

	UFUNCTION()
	void ReturnMainMenu();

};
