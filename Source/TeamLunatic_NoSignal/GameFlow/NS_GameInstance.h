#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h" 
#include "EGameModeType.h"  
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "NS_GameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCreateSessionSuccess);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionListReceived, const TArray<TSharedPtr<FJsonObject>>&);

class UNS_UIManager;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UNS_GameInstance();
	virtual void Init() override;

	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	FString CurrentSaveSlotName;

	void SetCurrentSaveSlot(FString SlotNameInfo);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
	UDataTable* GlobalItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_UIManager* GetUIManager() const { return NS_UIManager; };

	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }

	void CreateDedicatedSessionViaHTTP(FName SessionName, int32 MaxPlayers);
	void OnCreateSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// HTTP 세션 리스트 요청
	void RequestSessionListFromServer();
	void OnReceiveSessionList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> WaitingRoom;

	// 세션 리스트 받아오면 UI에서 처리 가능하도록 이벤트 델리게이트
	FOnSessionListReceived OnSessionListReceived;

	UPROPERTY()
	UNS_UIManager* NS_UIManager;

	UPROPERTY()
	TSubclassOf<UNS_UIManager> UIManagerClass;

	bool bIsSinglePlayer = true;

private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;
};
