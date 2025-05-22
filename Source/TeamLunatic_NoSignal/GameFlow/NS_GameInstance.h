#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EGameModeType.h"  
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreateSessionSuccess);

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FOnCreateSessionSuccess OnCreateSessionSuccess;

	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	FString CurrentSaveSlotName;

	// ���� ��� ����
	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }

	void CreateSession(APlayerController* PC, int32 NumConnections, bool bIsLAN);
	void SetCurrentSaveSlot(FString SaveSlotName);
private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
};
