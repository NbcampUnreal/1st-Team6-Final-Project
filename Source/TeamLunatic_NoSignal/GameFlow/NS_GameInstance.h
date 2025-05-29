#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EGameModeType.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreateSessionSuccess);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FOnCreateSessionSuccess OnCreateSessionSuccess;

	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }
	void CreateDedicatedSessionViaHTTP(FName SessionName, bool bIsLAN, int32 MaxPlayers);
	void OnCreateSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> WaitingRoom;

	void FindSessions(bool bIsLAN);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchSuccess, const TArray<FOnlineSessionSearchResult>&);
	FOnSessionSearchSuccess OnSessionSearchSuccess;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool);
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
