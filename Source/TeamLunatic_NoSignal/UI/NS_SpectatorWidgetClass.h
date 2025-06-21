#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_SpectatorWidgetClass.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_SpectatorWidgetClass : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* LeftButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RightButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;

	int32 CurrentIndex = 0;

	UFUNCTION()
	void OnLeftClicked();

	UFUNCTION()
	void OnRightClicked();

	UFUNCTION()
	void OnQuitClicked();

	void SpectatePlayerAtIndex(int32 Index);
};
