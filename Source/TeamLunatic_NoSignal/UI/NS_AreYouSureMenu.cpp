// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_AreYouSureMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/BackgroundBlur.h"
#include "Components/Image.h"

void UNS_AreYouSureMenu::OnClickedYesSelected()
{

}

void UNS_AreYouSureMenu::OnClickedNoSelected()
{

}

void UNS_AreYouSureMenu::NativeConstruct()
{
	Super::NativeConstruct();
	//if (YesButton)
	//	YesButton->OnClicked.AddDynamic(this, &UNS_AreYouSureMenu::OnClickedYesSelected);
	//if (NoButton)
	//	NoButton->OnClicked.AddDynamic(this, &UNS_AreYouSureMenu::OnClickedNoSelected);
}
