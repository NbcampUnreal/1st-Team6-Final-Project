// NS_DebugStatusWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_DebugStatusWidget.generated.h"

class UProgressBar;
class ANS_PlayerCharacterBase;
class UNS_StatusComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_DebugStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HungerBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ThirstBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* FatigueBar;

private:
	// 캐스트용 플레이어 캐릭터
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;
	// 캐스트용 스탯컴포넌트
	UPROPERTY()
	UNS_StatusComponent* StatusComp;
};
