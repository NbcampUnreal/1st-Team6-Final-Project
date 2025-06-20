#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_PlayerController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANS_PlayerController();

	UFUNCTION(Client, Reliable)
	void PlayTracked();

	UFUNCTION(Client, Reliable)
	void ClientShowGameOverUI();

	// 클라이언트 RPC: 게임 오버 시 입력 모드 변경 (마우스 활성화, 입력 차단)
	UFUNCTION(Client, Reliable)
	void ClientSetGameOverInputMode();

protected:
	virtual void BeginPlay() override;
	void SetupInputComponent();

	UFUNCTION()
	void ToggleInGameMenu();
	UFUNCTION()
	void TestGameOver();
	UFUNCTION()
	void TestGameMsg();


	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	USoundBase* ChaseStartSoundCue;

};
