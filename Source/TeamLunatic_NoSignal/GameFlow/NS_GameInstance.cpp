#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UI/NS_LoadingScreen.h"
#include "UI/NS_UIManager.h"
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
    static ConstructorHelpers::FClassFinder<UNS_UIManager> BP_UIManager(TEXT("/Game/UI/Blueprints/BP_NS_UIManager.BP_NS_UIManager_C"));
    if (BP_UIManager.Succeeded())
        UIManagerClass = BP_UIManager.Class;
    else
        UIManagerClass = UNS_UIManager::StaticClass();

    static ConstructorHelpers::FClassFinder<UUserWidget> BP_LoadingWait(TEXT("/Game/UI/Blueprints/BP_Wait.BP_Wait_C"));
    if (BP_LoadingWait.Succeeded())
        WaitClass = BP_LoadingWait.Class;

    static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeMulti(TEXT("/Game/GameFlowBP/BP_NS_MultiPlayMode.BP_NS_MultiPlayMode_C"));
    static ConstructorHelpers::FClassFinder<AGameModeBase> IncludeSingle(TEXT("/Game/GameFlowBP/BP_NS_SinglePlayMode.BP_NS_SinglePlayMode_C"));
}

void UNS_GameInstance::Init()
{
    Super::Init();

    if (UIManagerClass)
    {
        NS_UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);
        if (NS_UIManager) NS_UIManager->InitUi(GetWorld());
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(LevelLoadCheckTimer,
            FTimerDelegate::CreateUObject(this, &UNS_GameInstance::CheckForLevelLoadComplete),
            2.0f, true); // 2초마다 체크
    }

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface())
        {
            SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UNS_GameInstance::OnSessionUserInviteAccepted);
        }
    }

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UNS_GameInstance::OnPreLoadMap);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNS_GameInstance::OnPostLoadMapWithWorld);
}

void UNS_GameInstance::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 정리 시작"));
    if (PersistentLoadingScreen)
    {
        if (PersistentLoadingScreen->IsInViewport())
            PersistentLoadingScreen->RemoveFromParent();
        PersistentLoadingScreen = nullptr;
    }
    if (UWorld* World = GetWorld())
        World->GetTimerManager().ClearTimer(LevelLoadCheckTimer);
    Super::Shutdown();
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: 완료"));
}

void UNS_GameInstance::SetCurrentSaveSlot(FString SlotNameInfo)
{
    CurrentSaveSlotName = SlotNameInfo;
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

void UNS_GameInstance::ShowWait()
{
    UE_LOG(LogTemp, Error, TEXT("ShowWait 호출됨 - 실제로는 아무것도 안함(NS_LoadingScreen 사용중)"));
    return;
}

void UNS_GameInstance::HideWait()
{
    if (WaitWidget && WaitWidget->IsInViewport())
    {
        WaitWidget->RemoveFromParent();
        UE_LOG(LogTemp, Error, TEXT("기존 WaitWidget 제거됨"));
    }
}

UNS_BaseMainMenu* UNS_GameInstance::GetMainMenu()
{
    if (!MainMenu && MainMenuClass)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            APlayerController* PC = World->GetFirstPlayerController();
            if (PC)
            {
                MainMenu = CreateWidget<UNS_BaseMainMenu>(PC, MainMenuClass);
                UE_LOG(LogTemp, Log, TEXT("MainMenu created in GetMainMenu()"));
            }
        }
    }
    return MainMenu;
}

void UNS_GameInstance::OnPreLoadMap(const FString& MapName)
{
    UE_LOG(LogTemp, Error, TEXT("=== OnPreLoadMap 호출됨: %s ==="), *MapName);
    if (MapName.Contains(TEXT("MainWorld")))
    {
        if (NS_UIManager) NS_UIManager->ShowLoadingScreen(GetWorld());
    }
}

void UNS_GameInstance::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
    if (!LoadedWorld) return;
    FString MapName = LoadedWorld->GetMapName();
    UE_LOG(LogTemp, Error, TEXT("=== OnPostLoadMapWithWorld 호출됨: %s ==="), *MapName);

    if (MapName.Contains(TEXT("MainWorld")))
    {
        if (NS_UIManager && IsValid(NS_UIManager) && IsValid(LoadedWorld))
        {
            if (LoadedWorld->HasBegunPlay() && LoadedWorld->AreActorsInitialized())
            {
                NS_UIManager->ShowLoadingScreen(LoadedWorld);
            }
        }
    }
}

void UNS_GameInstance::CheckForLevelLoadComplete()
{
    UWorld* World = GetWorld();
    if (!World || bFrameRateCheckStarted) return;
    if (!NS_UIManager || !IsValid(NS_UIManager))
    {
        UE_LOG(LogTemp, Error, TEXT("CheckForLevelLoadComplete: UIManager가 유효하지 않습니다"));
        return;
    }
    FString CurrentLevelName = World->GetMapName();
    if (CurrentLevelName.Contains(TEXT("MainWorld")) && World->HasBegunPlay())
    {
        UE_LOG(LogTemp, Warning, TEXT("CheckForLevelLoadComplete: MainWorld 레벨 로드 완료 감지"));
        World->GetTimerManager().ClearTimer(LevelLoadCheckTimer);
        bFrameRateCheckStarted = true;
        StartPostLevelLoadFrameRateCheck();
    }
}

void UNS_GameInstance::StartPostLevelLoadFrameRateCheck()
{
    UE_LOG(LogTemp, Warning, TEXT("StartPostLevelLoadFrameRateCheck: 레벨 로드 완료 후 프레임률 체크 시작"));
    if (!NS_UIManager || !IsValid(NS_UIManager)) { NS_UIManager = nullptr; return; }
    UWorld* World = GetWorld();
    if (!World) return;
    if (PersistentLoadingScreen)
    {
        float CurrentProgress = PersistentLoadingScreen->GetCurrentProgress();
        if (!PersistentLoadingScreen->IsLoadingComplete())
            PersistentLoadingScreen->StartLoading();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("영구 로딩 스크린이 없습니다"));
    }
}

void UNS_GameInstance::CreatePersistentLoadingScreen()
{
    UE_LOG(LogTemp, Warning, TEXT("CreatePersistentLoadingScreen: 함수 호출됨"));
    UWorld* World = GetWorld();
    if (!World) { UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: World가 null입니다")); return; }
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) { UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: PlayerController가 null입니다")); return; }
    if (!NS_UIManager) { UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: UIManager가 null입니다")); return; }
    TSubclassOf<UNS_LoadingScreen> LoadingScreenClass = NS_UIManager->GetLoadingScreenClass();
    if (!LoadingScreenClass) { UE_LOG(LogTemp, Error, TEXT("CreatePersistentLoadingScreen: LoadingScreenClass가 null입니다")); return; }
    if (PersistentLoadingScreen)
    {
        PersistentLoadingScreen->RemoveFromParent();
        PersistentLoadingScreen = nullptr;
    }
    PersistentLoadingScreen = CreateWidget<UNS_LoadingScreen>(PC, LoadingScreenClass);
    if (PersistentLoadingScreen)
    {
        PersistentLoadingScreen->AddToViewport(10000);
        PersistentLoadingScreen->SetVisibility(ESlateVisibility::Visible);
        PersistentLoadingScreen->InitializeLoadingScreen();
        PersistentLoadingScreen->StartLoading();
        FInputModeUIOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("영구 로딩 스크린 생성 실패"));
    }
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
