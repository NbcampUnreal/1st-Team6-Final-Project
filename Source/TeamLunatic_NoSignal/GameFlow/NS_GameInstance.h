#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EGameModeType.h"  
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "NS_GameInstance.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }

	void CreateSession(FName SessionName, bool bIsLAN, int32 MaxPlayers);

private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	TSharedPtr<FOnlineSessionSettings> SessionSettings;
};
