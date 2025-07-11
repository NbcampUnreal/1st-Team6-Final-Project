// NS_PlayerWidget.cpp - 단순화된 플레이어 UI 위젯

#include "UI/InGame/NS_PlayerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UNS_PlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Crosshair)
    {
        Crosshair->SetVisibility(ESlateVisibility::Visible);
    }
}

void UNS_PlayerWidget::UpdateHealth(int32 CurrentHealth, int32 MaxHealth)
{
    if (HealthText)
    {
        // 현재 체력만 표시
        FText HealthDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentHealth));
        HealthText->SetText(HealthDisplayText);
    }
}

void UNS_PlayerWidget::UpdateStamina(int32 CurrentStamina, int32 MaxStamina)
{
    if (StaminaText)
    {
        FText StaminaDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentStamina));
        StaminaText->SetText(StaminaDisplayText);
    }
}

void UNS_PlayerWidget::SetCrosshairVisibility(bool bVisible)
{
    if (Crosshair)
    {
        Crosshair->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}