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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	USoundBase* ChaseStartSoundCue;
};
