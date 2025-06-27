#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "NS_ReadyUI.generated.h"

/**
 * 게임 시작 전 플레이어 준비 상태를 표시하는 UI 위젯 클래스
 * 플레이어들의 준비 상태를 보여주고 Ready/Quit 버튼을 제공합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ReadyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 위젯이 생성될 때 호출되는 함수 - 버튼 이벤트 바인딩 */
	virtual void NativeConstruct() override;

	/** 매 프레임마다 호출되는 함수 - 플레이어 상태 업데이트 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** 레디 버튼 눌렀을 때 호출될 함수 - 플레이어의 준비 상태를 토글 */
	UFUNCTION()
	void OnReadyButtonClicked();

	/** 나가기 버튼 눌렀을 때 호출될 함수 - 세션에서 나가거나 세션 파괴 */
	UFUNCTION()
	void OnQuitButtonClicked();

	/** 플레이어 상태 목록 갱신 함수 - 모든 플레이어의 준비 상태를 UI에 표시 */
	void UpdatePlayerStatusList();

protected:
	/** 메인 컨테이너 박스 */
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* RootBox;

	/** 준비 상태 토글 버튼 */
	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyButton;

	/** 세션 나가기 버튼 */
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;
	
	/** 나가기 버튼 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuitText;

	/** 플레이어 1 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player0;

	/** 플레이어 2 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player1;

	/** 플레이어 3 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player2;

	/** 플레이어 4 이름 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player3;

	/** 플레이어 1 준비 상태 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status0;

	/** 플레이어 2 준비 상태 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status1;

	/** 플레이어 3 준비 상태 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status2;

	/** 플레이어 4 준비 상태 텍스트 */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status3;

	// 플레이어의 현재 준비 상태를 저장하는 변수
	UPROPERTY()
	bool bIsPlayerReady = false;
};
