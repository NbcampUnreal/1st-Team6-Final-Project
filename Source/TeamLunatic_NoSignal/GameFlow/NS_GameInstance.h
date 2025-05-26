#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EGameModeType.h"  
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreateSessionSuccess);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// == 세션 생성 담당 부분들 == 
	FOnCreateSessionSuccess OnCreateSessionSuccess;

	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }
	void CreateSession(FName SessionName, bool bIsLAN, int32 MaxPlayers);

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> WaitingRoom;

	// == 세션 검색 담당 부분들 == 
	void FindSessions(bool bIsLAN);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchSuccess, const TArray<FOnlineSessionSearchResult>&);
	FOnSessionSearchSuccess OnSessionSearchSuccess;

	// == 세션 조인 담당 
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool /*bWasSuccessful*/);

	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	FOnJoinSessionComplete OnJoinSessionComplete;

private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	void OnFindSessionsComplete(bool bWasSuccessful);
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	void OnJoinSessionCompleteInternal(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
}; 
