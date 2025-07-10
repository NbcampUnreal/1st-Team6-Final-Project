#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_PlayerController.generated.h"

class UNS_Msg_GameOver;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 마우스 커서 숨기고
	// 클릭 이벤트 활성화 하고
	// 마우스 오버 이벤트 활성화
	ANS_PlayerController();

	// 추적자한테 플레이어가 추적 당할때 사운드 재생
	UFUNCTION(Client, Reliable)
	void PlayTracked();

	// 플레이어가 사망 게임이 종료될때 호출
	void HandleGameOver(bool bPlayerSurvived);

	// 데미지 받으면 화면에 피격 효과 표시
	UFUNCTION(Client, Reliable)
	void Client_ShowHitEffect();

	// 플레이어 TipHUD 함수 
	void UpdateTipHUD(const FText& Message);

	// 플레이어 체력 UI 업데이트 함수
	void UpdatePlayerHealthUI();

	// 플레이어 스태미너 UI 업데이트 함수
	void UpdatePlayerStaminaUI();


protected:
	virtual void BeginPlay() override;
	void SetupInputComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	USoundBase* ChaseStartSoundCue;
};
