// NS_ReadyUI.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "NS_ReadyUI.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ReadyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 레디 버튼 눌렀을 때 호출될 함수
	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	// 플레이어 상태 갱신용 함수
	void UpdatePlayerStatusList();

protected:

	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuitText;

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

	UPROPERTY(meta = (BindWidget))
	class UImage* image_0;

	UPROPERTY(meta = (BindWidget))
	class UImage* image_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* image_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* image_3;
};
