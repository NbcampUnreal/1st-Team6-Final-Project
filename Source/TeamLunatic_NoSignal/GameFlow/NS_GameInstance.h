#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "EGameModeType.h"
#include "NS_ReadyUI.h"
#include "UI/NS_BaseMainMenu.h"
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchComplete, const TArray<FOnlineSessionSearchResult>&);

class UNS_UIManager;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UNS_GameInstance();
    virtual void Init() override;
    virtual void Shutdown() override;
    void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);
    void SetCurrentSaveSlot(FString SlotNameInfo);
    UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
    FString CurrentSaveSlotName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
    UDataTable* GlobalItemDataTable;

    UFUNCTION(BlueprintCallable, Category = "UI")
    UNS_UIManager* GetUIManager() const { return NS_UIManager; };

    UFUNCTION(BlueprintCallable, Category = "Loading")
    void StartPostLevelLoadFrameRateCheck();

    UFUNCTION(BlueprintCallable, Category = "Loading")
    void CreatePersistentLoadingScreen();

    FTimerHandle LevelLoadCheckTimer;
    void CheckForLevelLoadComplete();
    void OnPreLoadMap(const FString& MapName);
    void OnPostLoadMapWithWorld(UWorld* LoadedWorld);

    bool bFrameRateCheckStarted = false;
    UPROPERTY()
    class UNS_LoadingScreen* PersistentLoadingScreen = nullptr;

    void SetGameModeType(EGameModeType Type);
    EGameModeType GetGameModeType() const { return GameModeType; }

    // ----- [Steam OnlineSubsystem 세션 관련 함수] -----
    void CreateSession(FName SessionName, int32 MaxPlayers);
    void FindSessions();
    void JoinSession(const FOnlineSessionSearchResult& SearchResult);

    void OnDestroySessionThenCreateSession(FName SessionName, bool bWasSuccessful);


    // 검색 결과 UI에 전달
    FOnSessionSearchComplete OnSessionSearchComplete;

    // 세션 검색 객체
    TSharedPtr<class FOnlineSessionSearch> SessionSearch;

    // 기존처럼 맵 소프트참조, UI매니저 등
    UPROPERTY(EditAnywhere, Category = "Level")
    TSoftObjectPtr<UWorld> WaitingRoom;

    UPROPERTY()
    UNS_UIManager* NS_UIManager;

    UPROPERTY()
    TSubclassOf<UNS_UIManager> UIManagerClass;

    bool bIsSinglePlayer = true;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> ReadyUIClass;
    UPROPERTY()
    class UNS_ReadyUI* ReadyUIInstance;

    void ShowReadyUI();
    void HideReadyUI();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> WaitClass;

    UPROPERTY()
    UUserWidget* WaitWidget;

    UFUNCTION(BlueprintCallable)
    void ShowWait();

    UFUNCTION(BlueprintCallable)
    void HideWait();

    UFUNCTION(BlueprintCallable, Category = "Network")
    void DestroyCurrentSession();

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UNS_BaseMainMenu* MainMenu;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_BaseMainMenu> MainMenuClass;
    UFUNCTION(BlueprintCallable, Category = "UI")
    UNS_BaseMainMenu* GetMainMenu();
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetMainMenu(UNS_BaseMainMenu* NewMainMenu) { MainMenu = NewMainMenu; }

private:
    EGameModeType GameModeType = EGameModeType::SinglePlayMode;
    FTimerHandle HeartbeatTimerHandle;

    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
