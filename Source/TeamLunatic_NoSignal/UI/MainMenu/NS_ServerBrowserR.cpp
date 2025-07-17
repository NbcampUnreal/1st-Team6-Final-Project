#include "UI/MainMenu/NS_ServerBrowserR.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/MainMenu/NS_ServerListingR.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Kismet/GameplayStatics.h"

#define SETTING_MAPNAME TEXT("MAPNAME")

void UNS_ServerBrowserR::NativeConstruct()
{
    Super::NativeConstruct();

    if (RefreshButton)
        RefreshButton->OnClicked.AddDynamic(this, &UNS_ServerBrowserR::OnRefreshButtonClicked);

    // GameInstance 델리게이트 바인딩
    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->OnSessionSearchComplete.RemoveAll(this);
        GI->OnSessionSearchComplete.AddUObject(this, &UNS_ServerBrowserR::OnSessionSearchResults);

    }

    RefreshServerList(); // 최초 자동 로드
}

void UNS_ServerBrowserR::OnRefreshButtonClicked()
{
    RefreshServerList();
}

void UNS_ServerBrowserR::RefreshServerList()
{
    UE_LOG(LogTemp, Log, TEXT("[ServerBrowserR] RefreshServerList 시작"));

    if (CircularThrobber_Image)
        CircularThrobber_Image->SetVisibility(ESlateVisibility::Visible);

    if (ServerVerticalBox)
    {
        int32 ChildrenCount = ServerVerticalBox->GetChildrenCount();
        ServerVerticalBox->ClearChildren();
        UE_LOG(LogTemp, Log, TEXT("[ServerBrowserR] 기존 서버 목록 정리됨 (%d개)"), ChildrenCount);
    }

    if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        GI->FindSessions(); // Steam 세션 검색
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ServerBrowserR] GameInstance 캐스팅 실패"));
    }
}

void UNS_ServerBrowserR::OnSessionSearchResults(const TArray<FOnlineSessionSearchResult>& Results)
{
    UE_LOG(LogTemp, Log, TEXT("[ServerBrowserR] 세션 검색 결과 받음. %d개"), Results.Num());

    if (ServerVerticalBox)
        ServerVerticalBox->ClearChildren();

    int32 AddedSessions = 0;
    for (const FOnlineSessionSearchResult& Result : Results)
    {
        FString Tag = TEXT("NONE");
        if (Result.Session.SessionSettings.Settings.Contains("NO_SIGNAL_TAG"))
        {
            Tag = Result.Session.SessionSettings.Settings["NO_SIGNAL_TAG"].Data.ToString();
        }

        FString RoomName = TEXT("NONE");
        if (Result.Session.SessionSettings.Settings.Contains("ROOMNAME"))
        {
            RoomName = Result.Session.SessionSettings.Settings["ROOMNAME"].Data.ToString();
        }

        UE_LOG(LogTemp, Warning, TEXT("[검색결과] 태그: %s | 방이름: %s | Owner: %s"),
            *Tag, *RoomName, *Result.Session.OwningUserName);

        if (Tag == "UNIQUE_TAG_202407")
        {
            AddServerEntry(Result);
            AddedSessions++;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[ServerBrowserR] %d개 세션이 UI에 추가됨"), AddedSessions);

    if (CircularThrobber_Image)
        CircularThrobber_Image->SetVisibility(ESlateVisibility::Hidden);
}



// SearchResult 기반 서버 엔트리 추가
void UNS_ServerBrowserR::AddServerEntry(const FOnlineSessionSearchResult& SessionResult)
{
    if (!ServerEntryClass) return;

    UNS_ServerListingR* Entry = CreateWidget<UNS_ServerListingR>(GetWorld(), ServerEntryClass);
    if (!Entry) return;

    Entry->SessionResult = SessionResult; // SearchResult 직접 저장

    FString ServerName = SessionResult.Session.OwningUserName;
    if (ServerName.IsEmpty() && SessionResult.Session.SessionSettings.Settings.Contains(SETTING_MAPNAME))
    {
        ServerName = SessionResult.Session.SessionSettings.Settings[SETTING_MAPNAME].Data.ToString();
    }

    Entry->ServerNameText->SetText(FText::FromString(ServerName));

    FString PlayerCount = FString::Printf(TEXT("%d/%d"),
        SessionResult.Session.SessionSettings.NumPublicConnections - SessionResult.Session.NumOpenPublicConnections,
        SessionResult.Session.SessionSettings.NumPublicConnections);
    Entry->PlayerContText->SetText(FText::FromString(PlayerCount));

    // 추가로 Ping 등도 표시 가능
    // FString Ping = FString::Printf(TEXT("%d ms"), SessionResult.PingInMs);
    // Entry->PingText->SetText(FText::FromString(Ping));

    ServerVerticalBox->AddChild(Entry);
}
