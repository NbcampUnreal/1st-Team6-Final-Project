// NS_InGameStartMenu.h - 게임 중 ESC 키를 눌러 표시되는 일시 정지 메뉴 위젯 클래스

#pragma once

#include "CoreMinimal.h"
#include "UI/NS_MasterMenuPanel.h"
#include "UI/NS_CommonType.h"
#include "NS_InGameStartMenu.generated.h"

// 전방 선언
class UNS_MenuButtonWidget;

/**
 * UNS_InGameStartMenu - 게임 중 일시 정지 메뉴를 담당하는 위젯 클래스
 * 이 클래스는 게임 중 ESC 키를 눌렀을 때 표시되는 메뉴를 관리합니다.
 * 게임 재개, 설정, 메인 메뉴로 돌아가기, 게임 종료 등의 기능을 제공합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InGameStartMenu : public UNS_MasterMenuPanel
{
    GENERATED_BODY()
public:
    /**
     * 위젯이 생성될 때 호출되는 함수
     * 버튼 이벤트 바인딩 및 초기 설정을 수행합니다.
     */
    virtual void NativeConstruct() override;
    
    /**
     * 메인 메뉴 참조를 설정하고 서브 메뉴들을 초기화하는 함수
     * @param NsMainMenu - 메인 메뉴 참조
     */
    virtual void Init(UNS_BaseMainMenu* NsMainMenu) override;
    
    /**
     * 위젯을 화면에 표시하는 함수
     * 표시 시 서브 메뉴들을 모두 숨깁니다.
     */
    virtual void ShowWidget() override;

    /**
     * 메인 메뉴로 돌아가는 버튼 클릭 이벤트 핸들러
     * 현재 세션을 종료하고 메인 타이틀 레벨을 로드합니다.
     */
    UFUNCTION()
    void On_MainMenuClicked();

protected:
    // 게임 재개 버튼
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Resume;

    // 설정 버튼
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Settings;

    // 컨트롤 설정 버튼
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Controls;

    // 메인 메뉴 버튼
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_MainMenu;

    // 게임 종료 버튼
    UPROPERTY(meta = (BindWidget))
    UNS_MenuButtonWidget* BP_Quit;

    /**
     * 게임 재개 버튼 클릭 이벤트 핸들러
     * 메뉴를 닫고 게임으로 돌아갑니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Widget")
    void OnResumeClicked();

    /**
     * 설정 버튼 클릭 이벤트 핸들러
     * 설정 메뉴를 표시합니다.
     */
    UFUNCTION()
    void OnSettingsClicked();

    /**
     * 컨트롤 설정 버튼 클릭 이벤트 핸들러
     * 컨트롤 설정 메뉴를 표시합니다.
     */
    UFUNCTION()
    void OnControlsClicked();
    
    /**
     * 온라인 세션 연결 해제 함수
     * 현재 멀티플레이어 세션이 있다면 종료합니다.
     */
    UFUNCTION()
    void OnDisconnectClicked();
    
    /**
     * 게임 종료 버튼 클릭 이벤트 핸들러
     * 세션을 종료하고 게임을 완전히 종료합니다.
     */
    UFUNCTION()
    void OnQuitClicked();
};