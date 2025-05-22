// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "NS_ServerBrowserR.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UCheckBox;
class UVerticalBox;
class UCircularThrobber;
class UNS_ServerListingR;
class UBorder;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ServerBrowserR : public UNS_MasterMenuPanel
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnRefreshButtonClicked();

    UFUNCTION()
    void OnUseLANCheckChanged(bool bIsChecked);

    void RefreshServerList();

protected:

    void HandleFindSessionsComplete(bool bWasSuccessful);

    void AddServerEntry(const FOnlineSessionSearchResult& SessionResult);

protected:
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)//, BlueprintReadOnly / BlueprintReadWrite
    UTextBlock* Subtitle;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock * Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UImage* PlayerArrow;
    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UImage* PingArrow;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UVerticalBox* ServerVerticalBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UCircularThrobber* CircularThrobber_Image;

    //UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
   // UNS_ServerListingR* BP_ServerListing_v1_ServerName;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UCheckBox* CheckBox_UseLAN;

    bool bUseLAN = false;

};
