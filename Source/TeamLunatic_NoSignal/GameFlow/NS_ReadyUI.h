// NS_ReadyUI.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_ReadyUI.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ReadyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 레디 버튼 눌렀을 때 호출될 함수
	UFUNCTION()
	void OnReadyButtonClicked();

	// 플레이어 상태 갱신용 함수
	void UpdatePlayerStatusList();

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* RootBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player0;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Player3;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status0;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Status3;
};
