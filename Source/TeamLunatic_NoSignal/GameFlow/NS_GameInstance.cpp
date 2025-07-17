#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EGameModeType.h"

#define SEARCH_PRESENCE TEXT("presence")
#define SETTING_MAPNAME TEXT("MAPNAME")
#define SETTING_ROOMNAME TEXT("ROOMNAME")
#define SETTING_GAME_TAG TEXT("NO_SIGNAL_TAG") 

FName PendingRoomName;
int32 PendingMaxPlayers = 0;
bool bPendingCreateSession = false;

UNS_GameInstance::UNS_GameInstance()
{
	// GameMode 강제 참조로 패키징 포함 유도
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeMulti(TEXT("/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C"));
	static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeSingle(TEXT("/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C"));
}

void UNS_GameInstance::Init()
{
    Super::Init();

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface())
        {
            SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UNS_GameInstance::OnSessionUserInviteAccepted);
        }
    }
}

void UNS_GameInstance::ShowReadyUI()
{
    UE_LOG(LogTemp, Warning, TEXT("ShowReadyUI() 진입"));
    if (!ReadyUIClass) { UE_LOG(LogTemp, Error, TEXT("ReadyUIClass is NULL!")); return; }
    if (!ReadyUIInstance)
    {
        ReadyUIInstance = CreateWidget<UNS_ReadyUI>(this, ReadyUIClass);
        UE_LOG(LogTemp, Warning, TEXT("ReadyUIInstance 생성: %s"), *GetNameSafe(ReadyUIInstance));
    }
    if (ReadyUIInstance && !ReadyUIInstance->IsInViewport())
    {
        ReadyUIInstance->AddToViewport();
        UE_LOG(LogTemp, Warning, TEXT("ReadyUIInstance AddToViewport 완료"));
    }
}

void UNS_GameInstance::HideReadyUI()
{
    if (ReadyUIInstance && ReadyUIInstance->IsInViewport())
        ReadyUIInstance->RemoveFromParent();
}

// =================== OnlineSubsystem (Steam) Listen Server/Session ===================

void UNS_GameInstance::CreateSession(FName RoomName, int32 MaxPlayers)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    // 기존 세션이 있으면 DestroySession 먼저!
    if (SessionInterface->GetNamedSession(NAME_GameSession))
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 기존 세션 존재, DestroySession부터 진행"));
        PendingRoomName = RoomName;
        PendingMaxPlayers = MaxPlayers;
        bPendingCreateSession = true;
        SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
            FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNS_GameInstance::OnDestroySessionThenCreateSession)
        );
        SessionInterface->DestroySession(NAME_GameSession);
        return;
    }

    // 세션 세팅 (방 이름 등)
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.NumPublicConnections = MaxPlayers;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;

    SessionSettings.Set(SETTING_GAME_TAG, FString("UNIQUE_TAG_202407"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    SessionSettings.Set(SETTING_ROOMNAME, RoomName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    if (SessionSettings.Settings.Contains(SETTING_GAME_TAG))
    {
        FString TagValue = SessionSettings.Settings[SETTING_GAME_TAG].Data.ToString();
        UE_LOG(LogTemp, Warning, TEXT("[방만들기] 태그값: %s"), *TagValue);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[방만들기] 태그값 없음!!!"));
    }


    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnCreateSessionComplete);
    SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

// DestroySession 완료 시 이어서 CreateSession 다시 호출
void UNS_GameInstance::OnDestroySessionThenCreateSession(FName SessionName, bool bWasSuccessful)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    if (bPendingCreateSession)
    {
        bPendingCreateSession = false;
        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] DestroySession 완료, CreateSession 재호출!"));
        CreateSession(PendingRoomName, PendingMaxPlayers);
    }
}

void UNS_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        FString MapURL = TEXT("/Game/Maps/WaitingRoom?listen");
        UGameplayStatics::OpenLevel(GetWorld(), FName(*MapURL), true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[GameInstance] CreateSession 실패"));
    }
}

void UNS_GameInstance::FindSessions()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = false;
    SessionSearch->MaxSearchResults = 50;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionSearch->QuerySettings.Set(SETTING_GAME_TAG, FString("UNIQUE_TAG_202407"), EOnlineComparisonOp::Equals);

    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnFindSessionsComplete);
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    TArray<FOnlineSessionSearchResult> Results;
    if (SessionSearch.IsValid())
        Results = SessionSearch->SearchResults;
    OnSessionSearchComplete.Broadcast(Results);
}

void UNS_GameInstance::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnJoinSessionComplete);
    SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
}

void UNS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    FString ConnectString;
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[GameInstance] GetResolvedConnectString 실패"));
    }
}

void UNS_GameInstance::DestroyCurrentSession()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (SessionInterface.IsValid())
        SessionInterface->DestroySession(NAME_GameSession);
}

void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
    GameModeType = Type;
    UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}

void UNS_GameInstance::OnSessionUserInviteAccepted(
    const bool bWasSuccessful,
    const int32 LocalUserNum,
    TSharedPtr<const FUniqueNetId> UserId,
    const FOnlineSessionSearchResult& InviteResult)
{
    UE_LOG(LogTemp, Warning, TEXT("[Steam] 초대 수락됨! 자동으로 세션 참가 시도."));

    if (bWasSuccessful && InviteResult.IsValid())
    {
        if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
        {
            if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface())
            {
                SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnJoinSessionComplete);
                SessionInterface->JoinSession(LocalUserNum, NAME_GameSession, InviteResult);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Steam] 초대 수락 실패 혹은 세션 정보 없음!"));
    }
}
