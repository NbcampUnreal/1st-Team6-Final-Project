#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "NS_ServerListingR.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ServerListingR : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnJoinButtonClicked();

    // Steam OnlineSubsystem 세션 검색 결과 (필수)
    FOnlineSessionSearchResult SessionResult;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* ServerNameText;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* PlayerContText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UButton* JoinServerButton;
};
