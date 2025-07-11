// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_InteractionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/Interface/NS_InteractionInterface.h"

/**
 * @brief 위젯이 초기화될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 설정 및 변수 초기화에 사용됩니다.
 */
void UNS_InteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// InteractionProgressBar의 PercentDelegate에 UpdateInteractionProgress 함수를 바인딩합니다.
	// 이를 통해 프로그레스 바의 진행률이 UpdateInteractionProgress 함수의 반환 값에 따라 자동으로 업데이트됩니다.
	InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

/**
 * @brief 위젯이 생성될 때 호출되는 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_InteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// KeyPressText의 초기 텍스트를 "Press"로 설정합니다.
	KeyPressText->SetText(FText::FromString("Press"));
	// CurrentInteractionDuration을 0.0으로 초기화합니다.
	CurrentInteractionDuration = 0.0f;
}

/**
 * @brief 위젯에 표시될 상호작용 데이터를 업데이트합니다.
 * @param InteractableData 업데이트할 상호작용 데이터 구조체 포인터입니다.
 *        이 함수는 위젯의 텍스트와 프로그레스 바를 갱신합니다.
 */
void UNS_InteractionWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
	// 상호작용 가능한 객체의 타입에 따라 위젯의 표시 방식을 변경합니다.
	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		// 픽업 가능한 아이템인 경우
		KeyPressText->SetText(NSLOCTEXT("InteractionWidget", "KeyPressText_Press", "눌러서")); // "눌러서"
		ActionText->SetText(NSLOCTEXT("InteractionWidget", "ActionText_Use", "획득한다"));     // "획득한다"
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed); // 프로그레스 바 숨김

		// 아이템 수량이 1개인 경우 수량 텍스트를 숨기고, 그렇지 않으면 수량을 표시합니다.
		if (InteractableData->Quantity == 1)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "QuantityText", "{0}개"),
				InteractableData->Quantity)); // "{수량}개"
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		break;

	case EInteractableType::Device:
		// 장치인 경우
		KeyPressText->SetText(NSLOCTEXT("InteractionWidget", "KeyPressText_Press", "눌러서")); // "눌러서"
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed); // 프로그레스 바 숨김
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);         // 수량 텍스트 숨김
		break;

	case EInteractableType::None:
		// 특정 타입이 지정되지 않은 경우 (예: 문)
		KeyPressText->SetText(NSLOCTEXT("InteractionWidget", "KeyPressText_Press", "눌러서")); // "눌러서"
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed); // 프로그레스 바 숨김
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);         // 수량 텍스트 숨김
		NameText->SetText(NSLOCTEXT("InteractionWidget", "NameText_Door", "문을"));         // "문을"
		ActionText->SetText(NSLOCTEXT("InteractionWidget", "ActionText_Use", "사용한다"));     // "사용한다"
		break;

		// 추후 다른 타입도 확장 가능
	default:
		break;
	}

	// 공통 텍스트 처리: InteractableData에 이름이나 액션이 설정되어 있으면 해당 텍스트를 업데이트합니다.
	// 이는 switch 문에서 설정된 기본값을 덮어쓸 수 있습니다.
	if (!InteractableData->Name.IsEmpty())
		NameText->SetText(InteractableData->Name);
	if (!InteractableData->Action.IsEmpty())
		ActionText->SetText(InteractableData->Action);
}

/**
 * @brief 상호작용 진행 바의 현재 값을 계산하여 반환하는 함수입니다.
 *        프로그레스 바의 퍼센티지를 업데이트하는 데 사용됩니다.
 * @return 0.0에서 1.0 사이의 상호작용 진행률입니다.
 *         현재는 항상 0.0을 반환하므로, 실제 진행률 로직이 필요합니다.
 */
float UNS_InteractionWidget::UpdateInteractionProgress()
{
	// TODO: 실제 상호작용 진행률을 계산하여 반환하도록 구현해야 합니다.
	// 예: CurrentInteractionDuration / MaxInteractionDuration
	return 0.0f;
}


