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
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchComplete, const TArray<FOnlineSessionSearchResult>&);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UNS_GameInstance();
    virtual void Init() override;
    void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
    UDataTable* GlobalItemDataTable;

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

    bool bIsSinglePlayer = true;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> ReadyUIClass;
    UPROPERTY()
    class UNS_ReadyUI* ReadyUIInstance;

    void ShowReadyUI();
    void HideReadyUI();

    UFUNCTION(BlueprintCallable, Category = "Network")
    void DestroyCurrentSession();

private:
    EGameModeType GameModeType = EGameModeType::SinglePlayMode;

    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
