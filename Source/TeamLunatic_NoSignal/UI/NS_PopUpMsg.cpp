// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_PopUpMsg.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/NS_BaseMainMenu.h"
#include "UI/NS_MasterMenuPanel.h"

void UNS_PopUpMsg::NativeConstruct()
{
	Super::NativeConstruct();
	if (YesButton)
		YesButton->OnClicked.AddDynamic(this, &UNS_PopUpMsg::OnYesButtonClicked);
}

void UNS_PopUpMsg::OnYesButtonClicked()
{
	OnSuccessOkButtonEvent.Broadcast();
	HideWidget();
}
