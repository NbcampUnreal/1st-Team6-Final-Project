#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_SpectatorWidgetClass.generated.h"

// 전방 선언: 컴파일 속도를 향상시킵니다.
class UButton;
class UTextBlock;
class APlayerState;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SpectatorWidgetClass : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RightButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName;

private:
	int32 CurrentIndex = 0;

	UPROPERTY()
	TArray<TObjectPtr<APlayerState>> AlivePlayerStates;

	UFUNCTION()
	void OnLeftClicked();

	UFUNCTION()
	void OnRightClicked();

	UFUNCTION()
	void OnQuitClicked();

	void UpdateAlivePlayers();

	void UpdateAndSpectateFirstPlayer();

	void SpectatePlayerAtIndex(int32 Index);
};