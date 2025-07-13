// NS_PlayerWidget.cpp - 단순화된 플레이어 UI 위젯

#include "UI/InGame/NS_PlayerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI/InGame/NS_InteractionPanel.h"
#include "Character/Interface/NS_InteractionInterface.h"

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

// 플레이어의 현재 체력과 최대 체력을 업데이트하여 UI에 표시
void UNS_PlayerWidget::UpdateHealth(int32 CurrentHealth)
{
    // HealthText(체력 텍스트)가 유효하면 현재 체력을 텍스트로 설정합니다.
    if (HealthText)
    {
        // 현재 체력만 표시
        FText HealthDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentHealth));
        HealthText->SetText(HealthDisplayText);
    }
}

// 플레이어의 현재 스태미나와 최대 스태미나를 업데이트하여 UI에 표시
void UNS_PlayerWidget::UpdateStamina(int32 CurrentStamina)
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

/**
 * @brief 상호작용 위젯을 표시합니다.
 */
void UNS_PlayerWidget::ShowInteractionWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("ShowInteractionWidget 호출됨"));
    
    // 상호작용 위젯 클래스 확인
    if (!InteractionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("InteractionWidgetClass가 설정되지 않았습니다!"));
        return; // 생성자에서 설정해야 함
    }
    
    // 상호작용 위젯이 없으면 생성
    if (!InteractionWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractionWidget 생성 시도"));
        InteractionWidget = CreateWidget<UNS_InteractionPanel>(this, InteractionWidgetClass);
        if (InteractionWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("InteractionWidget 생성 성공, AddToViewport 시도"));
            InteractionWidget->AddToViewport(100); // 높은 Z-Order로 변경하여 확인
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("InteractionWidget 생성 실패!"));
            return;
        }
    }
    
    // 상호작용 위젯이 있으면 표시
    if (InteractionWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractionWidget 표시 설정"));
        InteractionWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

/**
 * @brief 상호작용 위젯을 숨깁니다.
 */
void UNS_PlayerWidget::HideInteractionWidget()
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

/**
 * @brief 상호작용 위젯을 업데이트합니다.
 * @param InteractableData 상호작용 가능한 오브젝트의 데이터입니다.
 */
void UNS_PlayerWidget::UpdateInteractionWidget(const FInteractableData* InteractableData)
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateInteractionWidget 호출됨"));
    
    if (!InteractableData)
    {
        UE_LOG(LogTemp, Error, TEXT("InteractableData가 유효하지 않습니다!"));
        return;
    }
    
    // 위젯이 없으면 생성
    if (!InteractionWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractionWidget이 없어서 ShowInteractionWidget 호출"));
        ShowInteractionWidget();
    }
    
    if (InteractionWidget)
    {
        if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
        {
            UE_LOG(LogTemp, Warning, TEXT("InteractionWidget이 Collapsed 상태여서 Visible로 변경"));
            InteractionWidget->SetVisibility(ESlateVisibility::Visible);
        }
        
        UE_LOG(LogTemp, Warning, TEXT("InteractionWidget UpdateWidget 호출"));
        InteractionWidget->UpdateWidget(InteractableData);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("InteractionWidget이 여전히 유효하지 않습니다!"));
    }
}

/**
 * @brief 상호작용 위젯 클래스를 설정합니다.
 * @param InWidgetClass 상호작용 위젯 클래스입니다.
 */
void UNS_PlayerWidget::SetInteractionWidgetClass(TSubclassOf<UNS_InteractionPanel> InWidgetClass)
{
    InteractionWidgetClass = InWidgetClass;
}