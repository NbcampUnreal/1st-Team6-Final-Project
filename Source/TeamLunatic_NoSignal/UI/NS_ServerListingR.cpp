// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_ServerListingR.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "NS_ServerBrowserR.h"

void UNS_ServerListingR::NativeConstruct()
{
	JoinServerButton->OnClicked.AddDynamic(this, &UNS_ServerListingR::OnJoinServerButtonClicked);
}
void UNS_ServerListingR::OnJoinServerButtonClicked()
{
   
}
