// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"                   
#include "NS_ServerListingR.generated.h"

class UTextBlock;
class UButton;
class UNS_ServerBrowserR;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ServerListingR : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnJoinButtonClicked();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* ServerNameText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* PlayerContText;

	FString CustomAddress;
	FString CustomServerName;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* JoinServerButton;

	FOnlineSessionSearchResult SessionResult;

};
