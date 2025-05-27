// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_ServerBrowserR.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Online/OnlineSessionNames.h>

void UNS_ServerBrowserR::NativeConstruct()
{
	Super::NativeConstruct();

    if (CheckBox_UseLAN)
        CheckBox_UseLAN->OnCheckStateChanged.AddDynamic(this, &UNS_ServerBrowserR::OnUseLANCheckChanged);

    if (RefreshButton)
        RefreshButton->OnClicked.AddDynamic(this, &UNS_ServerBrowserR::OnRefreshButtonClicked);

    RefreshServerList(); // 최초 자동 로드
}

void UNS_ServerBrowserR::OnUseLANCheckChanged(bool bIsChecked)
{
    bUseLAN = bIsChecked;
}

void UNS_ServerBrowserR::OnRefreshButtonClicked()
{
    RefreshServerList();
}

/* == RefreshServerList()설명 ==
    1.기존 서버 목록을 지우고
    2.LAN 설정에 따라 서버(세션) 검색을 시작하며
    3.결과가 나오면 콜백 함수에서 UI에 서버 리스트를 표시하는 구조*/
void UNS_ServerBrowserR::RefreshServerList()
{
    if (CircularThrobber_Image)
        CircularThrobber_Image->SetVisibility(ESlateVisibility::Visible);

    if (ServerVerticalBox)
        ServerVerticalBox->ClearChildren();

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        //서브시스템이 유효하다면, 세션 기능을 담당하는 인터페이스(SessionInterface) 를 가져옴
        //이게 실제로 CreateSession(), FindSessions(), JoinSession() 등을 실행하는 주체
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
        if (Sessions.IsValid())
        {
            SessionSearch = MakeShareable(new FOnlineSessionSearch()); //세션 검색 요청을 위한 설정 객체 생성/ 이 객체에 "어떤 세션을 찾을 것인지" 옵션을 넣게 됨
            SessionSearch->MaxSearchResults = 100;//최대 몇 개까지 서버(세션)를 검색할지 지정 (보통 100이면 충분)
            SessionSearch->bIsLanQuery = bUseLAN;//rue면 LAN 모드로만 검색 (같은 네트워크 안의 호스트만 탐색됨)/false면 Steam/EOS 등 외부 세션도 검색 가능
           
            //온라인 상태 필터 설정 (Steam 등에서 사용됨) /"Presence"가 있는 세션만 검색 대상이 됨/ 일반적으로 필수 옵션이라 넣어두는 편
            SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

            //세션 검색이 끝났을 때 실행할 함수 등록 (HandleFindSessionsComplete)
            Sessions->OnFindSessionsCompleteDelegates.AddUObject(this, &UNS_ServerBrowserR::HandleFindSessionsComplete);
           
            //실제 세션 검색 시작 (FindSessions())
            Sessions->FindSessions(0, SessionSearch.ToSharedRef());
        }
    }
}

void UNS_ServerBrowserR::HandleFindSessionsComplete(bool bWasSuccessful)
{
    if (CircularThrobber_Image)
        CircularThrobber_Image->SetVisibility(ESlateVisibility::Hidden);

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (!OnlineSub) return;

    IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    if (Sessions.IsValid())
    {
        Sessions->ClearOnFindSessionsCompleteDelegates(this);
    }

    if (bWasSuccessful && SessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
        {
            AddServerEntry(Result);
        }
    }
}

void UNS_ServerBrowserR::AddServerEntry(const FOnlineSessionSearchResult& SessionResult)
{
    // 예시: 서버 이름 가져오기
    FString ServerName = SessionResult.Session.OwningUserName;

    // 여기서 CreateWidget<UNS_ServerListingWidget>() 등으로 서버 엔트리 생성해서 ServerVerticalBox->AddChild()
    // 실제 위젯 클래스와 텍스트 바인딩 필요
}