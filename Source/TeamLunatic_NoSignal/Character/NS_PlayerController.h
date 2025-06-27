#pragma once

#include "CoreMinimal.h"
#include "UI/NS_PlayerHUD.h"
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


protected:
	virtual void BeginPlay() override;
	void SetupInputComponent();

	UFUNCTION()
	void ToggleInGameMenu();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UNS_Msg_GameOver> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	USoundBase* ChaseStartSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_PlayerHUD> PlayerHUDClass;

	UPROPERTY()
	TObjectPtr<UNS_PlayerHUD> PlayerHUDWidget;

};
