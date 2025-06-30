#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h" 
#include "EGameModeType.h"
#include "NS_ReadyUI.h"
#include "HttpModule.h"
#include "UI/NS_BaseMainMenu.h"
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
	virtual void Shutdown() override;

	//UFUNCTION()
	//void OnLevelLoaded(UWorld* LoadedWorld);

	void SetCurrentSaveSlot(FString SlotNameInfo);

	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	FString CurrentSaveSlotName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Config")
	UDataTable* GlobalItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_UIManager* GetUIManager() const { return NS_UIManager; };

	// 레벨 로드 완료 후 프레임률 체크 시작
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StartPostLevelLoadFrameRateCheck();

	// 영구 로딩 스크린 생성 (레벨 전환에도 살아남음)
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void CreatePersistentLoadingScreen();

	// 레벨 로드 완료 감지를 위한 타이머 핸들
	FTimerHandle LevelLoadCheckTimer;

	// 레벨 로드 완료 체크 함수
	void CheckForLevelLoadComplete();

	// 레벨 전환 시작 전 호출 (인게임 화면 숨기기)
	void OnPreLoadMap(const FString& MapName);

	// 레벨 전환 완료 후 호출
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);

	// 프레임률 체크가 이미 시작되었는지 플래그
	bool bFrameRateCheckStarted = false;

	// 레벨 전환에도 살아남는 로딩 스크린 (GameInstance에서 관리)
	UPROPERTY()
	class UNS_LoadingScreen* PersistentLoadingScreen = nullptr;

	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }

	void CreateDedicatedSessionViaHTTP(FName SessionName, int32 MaxPlayers);
	void OnCreateSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// HTTP 세션 리스트 요청
	void RequestSessionListFromServer();
	void OnReceiveSessionList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// HTTP 세션 리스트 숨김
	void RequestUpdateSessionStatus(int32 Port, FString Status); 
	void OnUpdateSessionStatusResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> WaitingRoom;

	// 세션 리스트 받아오면 UI에서 처리 가능하도록 이벤트 델리게이트
	FOnSessionListReceived OnSessionListReceived;

	UPROPERTY()
	UNS_UIManager* NS_UIManager;

	UPROPERTY()
	TSubclassOf<UNS_UIManager> UIManagerClass;

	bool bIsSinglePlayer = true;

	void SendHeartbeat();

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

	int32 MyServerPort = -1;
	// 메인 메뉴 위젯 인스턴스
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UNS_BaseMainMenu* MainMenu;

	// 메인 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_BaseMainMenu> MainMenuClass;

	// 메인 메뉴 참조 반환 함수 (동적 생성 기능 포함)
	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_BaseMainMenu* GetMainMenu();

	// MainMenu 설정 함수 추가
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetMainMenu(UNS_BaseMainMenu* NewMainMenu) { MainMenu = NewMainMenu; }

private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;
	FTimerHandle HeartbeatTimerHandle;
};