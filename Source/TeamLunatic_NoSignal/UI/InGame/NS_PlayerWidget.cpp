// NS_PlayerWidget.cpp - 단순화된 플레이어 UI 위젯

#include "UI/InGame/NS_PlayerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_PlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 크로스헤어 이미지가 존재하면 초기에는 보이도록 설정합니다.
    if (Crosshair)
    {
        Crosshair->SetVisibility(ESlateVisibility::Visible);
    }
}

/**
 * @brief 플레이어의 현재 체력과 최대 체력을 업데이트하여 UI에 표시합니다.
 * @param CurrentHealth 현재 플레이어의 체력입니다.
 * @param MaxHealth 플레이어의 최대 체력입니다.
 */
void UNS_PlayerWidget::UpdateHealth(int32 CurrentHealth, int32 MaxHealth)
{
    // HealthText(체력 텍스트)가 유효하면 현재 체력을 텍스트로 설정합니다.
    if (HealthText)
    {
        // 현재 체력만 표시
        FText HealthDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentHealth));
        HealthText->SetText(HealthDisplayText);
    }
}

/**
 * @brief 플레이어의 현재 스태미나와 최대 스태미나를 업데이트하여 UI에 표시합니다.
 * @param CurrentStamina 현재 플레이어의 스태미나입니다.
 * @param MaxStamina 플레이어의 최대 스태미나입니다.
 */
void UNS_PlayerWidget::UpdateStamina(int32 CurrentStamina, int32 MaxStamina)
{
    // StaminaText(스태미나 텍스트)가 유효하면 현재 스태미나를 텍스트로 설정합니다.
    if (StaminaText)
    {
        FText StaminaDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentStamina));
        StaminaText->SetText(StaminaDisplayText);
    }
}

/**
 * @brief 크로스헤어의 가시성을 설정합니다.
 * @param bVisible 크로스헤어를 보이게 할지(true) 숨기게 할지(false) 여부입니다.
 */
void UNS_PlayerWidget::SetCrosshairVisibility(bool bVisible)
{
    // Crosshair(크로스헤어 이미지)가 유효하면 bVisible 값에 따라 가시성을 설정합니다.
    if (Crosshair)
    {
        Crosshair->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}