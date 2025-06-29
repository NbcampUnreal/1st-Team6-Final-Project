// NS_UIManager.h - 게임의 모든 UI 위젯을 관리하는 중앙 매니저 클래스

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Ticker.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "NS_UIManager.generated.h"

// 전방 선언
class UNS_BaseMainMenu;
class UNS_Msg_GameOver;
class UNS_InGameMsg;
class UNS_PlayerHUD;
class UNS_CircleProgressBar;
class UNS_InGameMenu;
class UNS_QuickSlotPanel;
class UNS_LoadingScreen;
class UNS_SpectatorWidgetClass;
class UNS_GameInstanceClass;

// 로딩 완료 시 호출될 델리게이트 선언
DECLARE_DELEGATE(FOnLoadingFinished);

// PlayerHUD가 준비되었을 때 호출될 멀티캐스트 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHUDReady, UNS_PlayerHUD*, PlayerHUD);

// UI 매니저 클래스 정의
UCLASS(Blueprintable)
class TEAMLUNATIC_NOSIGNAL_API UNS_UIManager : public UObject
{
    GENERATED_BODY()
    
public:
    // 생성자
    UNS_UIManager();

    // UI 시스템 초기화 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void InitUi(UWorld* World);

    // 퀵슬롯 패널 위젯 반환
    UNS_QuickSlotPanel* GetQuickSlotPanel();

    // 인게임 메뉴 위젯이 뷰포트에 있는지 확인
    bool IsInViewportInGameMenuWidget();

    // 인게임 메뉴 위젯 반환
    UFUNCTION(BlueprintCallable, Category = "UI")
    UNS_InGameMenu* GetNS_MainMenuWidget()const { return InGameMenuWidget; }
    
    // 플레이어 HUD 위젯 반환
    UNS_PlayerHUD* GetPlayerHUDWidget() const { return NS_PlayerHUDWidget; }

    // 인게임 메뉴 위젯 표시
    UFUNCTION(BlueprintCallable, Category = "UI")
    bool ShowInGameMenuWidget(UWorld* World);
    
    // 로드 게임 위젯 표시
    void ShowLoadGameWidget(UWorld* World);
    
    // 인게임 메뉴 위젯 숨기기
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideInGameMenuWidget(UWorld* World);

    // 게임 오버 위젯 표시
    UFUNCTION(BlueprintCallable, Category = "UI")
    bool ShowGameOverWidget(UWorld* World);


UFUNCTION(BlueprintCallable, Category = "UI")
TSubclassOf<UUserWidget> GetOpenMapClass() const { return OpenMapClass; }

UPROPERTY(EditAnywhere, Category = "UI")
TSubclassOf<UUserWidget> OpenMapClass;

    
    // 게임 오버 위젯 숨기기
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideGameOverWidget(UWorld* World);

    // 히트 이펙트 위젯 표시 (피격 시 화면 효과)
    UFUNCTION(BlueprintCallable)
    void ShowHitEffectWidget(UWorld* World);

    // 게임 메시지 위젯 표시
    UFUNCTION(BlueprintCallable, Category = "UI")
    bool ShowGameMsgWidget(FString& GameMsg, UWorld* World);
    
    // 게임 메시지 위젯 숨기기
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideGameMsgWidget(UWorld* World);

    // 플레이어 HUD 위젯 표시
    UFUNCTION(BlueprintCallable, Category = "UI")
    bool ShowPlayerHUDWidget(UWorld* World);
    
    // 플레이어 HUD 위젯 숨기기
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HidePlayerHUDWidget(UWorld* World);

    // 게임 및 UI 입력 모드 설정
    void SetFInputModeGameAndUI(APlayerController* PC, UUserWidget* Widget);
    
    // 게임 전용 입력 모드 설정
    void SetFInputModeGameOnly(APlayerController* PC);

    // 로딩 화면 표시
    void LoadingScreen(UWorld* World);

    // 로딩 화면 표시 (레벨 전환용)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowLoadingScreen(UWorld* World);

    // 로딩 화면 숨기기 및 게임 모드 복원
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideLoadingScreen(UWorld* World);

    // 로딩 스크린에서 실제 프레임률 체크 시작
    UFUNCTION(BlueprintCallable, Category = "UI")
    void StartFrameRateCheck();

    // LoadingScreenClass getter 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    TSubclassOf<UNS_LoadingScreen> GetLoadingScreenClass() const { return NS_LoadingScreenClass; }

    // 로딩 UI 닫기
    void CloseLoadingUI();

    // 로딩 프로세스 완료 처리
    void CompleteLoadingProcess();

    // 로딩 완료 시 호출될 델리게이트
    FOnLoadingFinished OnLoadingFinished;
    
    // 관전자 위젯 표시
    UFUNCTION(BlueprintCallable)
    bool ShowSpectatorWidget(UWorld* World);

    // PlayerHUD 준비 완료 시 호출될 멀티캐스트 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnPlayerHUDReady OnPlayerHUDReady;

    // 외부에서 생성된 PlayerHUD를 UIManager에 등록하는 함수
    void SetPlayerHUDWidget(UNS_PlayerHUD* InHUD);

protected:
    // 인게임 메뉴 위젯 인스턴스
    UPROPERTY()
    UNS_InGameMenu* InGameMenuWidget;
    
    // 게임 오버 메시지 위젯 인스턴스
    UNS_Msg_GameOver* NS_Msg_GameOveWidget;
    
    // 인게임 메시지 위젯 인스턴스
    UNS_InGameMsg* NS_InGameMsgWidget;
    
    // 플레이어 HUD 위젯 인스턴스
    UNS_PlayerHUD* NS_PlayerHUDWidget;
    
    // 로딩 화면 위젯 인스턴스
    UNS_LoadingScreen* NS_LoadingScreen;

    // 인게임 메뉴 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_BaseMainMenu> InGameMenuWidgetClass;

    // 게임 오버 메시지 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_Msg_GameOver> NS_MsgGameOverWidgetClass;

    // 인게임 메시지 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_InGameMsg> NS_InGameMsgWidgetClass;

    // 플레이어 HUD 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_PlayerHUD> NS_PlayerHUDWidgetClass;

    // 로딩 화면 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_LoadingScreen> NS_LoadingScreenClass;

    // 히트 이펙트 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> HitEffectWidgetClass;

    // 관전자 위젯 인스턴스
    UPROPERTY()
    UNS_SpectatorWidgetClass* SpectatorWidget;

    // 관전자 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNS_SpectatorWidgetClass> SpectatorWidgetClass;

private:
    // 추가 private 멤버가 필요한 경우 여기에 선언
};