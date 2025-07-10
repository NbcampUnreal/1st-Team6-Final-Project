// NS_PlayerWidget.h - 단순화된 플레이어 UI 위젯

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_PlayerWidget.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_PlayerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    
    // UI 업데이트 함수들
    void UpdateHealth(int32 CurrentHealth, int32 MaxHealth);
    void UpdateStamina(int32 CurrentStamina, int32 MaxStamina);
    void SetCrosshairVisibility(bool bVisible); 

    // 위젯 바인딩
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StaminaText;

    UPROPERTY(meta = (BindWidget))
    class UImage* Crosshair;
};