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
	ANS_PlayerController();

	UFUNCTION(Client, Reliable)
	void PlayTracked();

	void HandleGameOver(bool bPlayerSurvived);

	UFUNCTION(Client, Reliable)
	void Client_ShowHitEffect();


	UFUNCTION(Client, Reliable)
	void Client_UpdateTipText(const FText& Message);

protected:
	virtual void BeginPlay() override;
	void SetupInputComponent();

	UFUNCTION()
	void ToggleInGameMenu();
	UFUNCTION()
	void TestGameOver();
	UFUNCTION()
	void TestGameMsg();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UNS_Msg_GameOver> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chase")
	USoundBase* ChaseStartSoundCue;

};
