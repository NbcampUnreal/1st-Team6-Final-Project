#pragma once

#include "CoreMinimal.h"
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
class TEAMLUNATIC_NOSIGNAL_API UNS_ServerBrowserR : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnRefreshButtonClicked();

    // Steam 세션 기반 서버 리스트 새로고침
    void RefreshServerList();

    // UI에 세션 엔트리 추가 (SearchResult 기반)
    void AddServerEntry(const FOnlineSessionSearchResult& SessionResult);

    // OnSessionSearchComplete 델리게이트에 바인딩해서 받는 함수
    void OnSessionSearchResults(const TArray<FOnlineSessionSearchResult>& Results);

protected:
    // 세션 검색 객체
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UBorder* Backround;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UTextBlock* Title;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UImage* PlayerArrow;
    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UImage* PingArrow;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UVerticalBox* ServerVerticalBox;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UCircularThrobber* CircularThrobber_Image;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UButton* RefreshButton;

    // 서버 엔트리 위젯 클래스 (SearchResult 기반)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UNS_ServerListingR> ServerEntryClass;
};
