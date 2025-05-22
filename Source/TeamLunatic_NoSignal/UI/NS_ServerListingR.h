// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "NS_ServerListingR.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ServerListingR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_First;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_Second;


	UPROPERTY(meta = (BindWidget))
	UButton* JoinServerButton;
	
};
