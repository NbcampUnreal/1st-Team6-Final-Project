#include "UI/InGame/NS_PlayerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI/InGame/NS_InteractionPanel.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Kismet/GameplayStatics.h"

void UNS_PlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 플레이어 캐릭터 찾기
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
    }
}

/**
 * @brief 상호작용 위젯을 표시합니다.
 */
void UNS_PlayerWidget::ShowInteractionWidget()
{
    // 상호작용 위젯 클래스 확인
    if (!InteractionWidgetClass)
    {
        return; 
    }
    
    // 상호작용 위젯이 없으면 생성
    if (!InteractionWidget)
    {
        InteractionWidget = CreateWidget<UNS_InteractionPanel>(this, InteractionWidgetClass);
        if (InteractionWidget)
        {
            InteractionWidget->AddToViewport(1); // 높은 Z-Order로 변경하여 확인
        }
        else
        {
            return;
        }
    }
    
    // 상호작용 위젯이 있으면 표시
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

// 상호작용 위젯을 숨김
void UNS_PlayerWidget::HideInteractionWidget()
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// 상호작용 위젯을 업데이트
void UNS_PlayerWidget::UpdateInteractionWidget(const FInteractableData* InteractableData)
{
    if (!InteractableData)
    {
        return;
    }
    
    // 위젯이 없으면 생성
    if (!InteractionWidget)
    {
        ShowInteractionWidget();
    }
    
    if (InteractionWidget)
    {
        if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
        {
            InteractionWidget->SetVisibility(ESlateVisibility::Visible);
        }
        
        InteractionWidget->UpdateWidget(InteractableData);
    }
}