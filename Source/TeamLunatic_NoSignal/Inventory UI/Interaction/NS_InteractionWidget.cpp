// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Interaction/NS_InteractionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interaction/InteractionInterface.h"

void UNS_InteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UNS_InteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KeyPressText->SetText(FText::FromString("Press"));
	CurrentInteractionDuration = 0.0f;
}

void UNS_InteractionWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		if (InteractableData->Quantity == 1)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "QuantityText", "x{0}"),
				InteractableData->Quantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		break;

	case EInteractableType::Device:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case EInteractableType::None:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		NameText->SetText(FText::FromString("Door"));
		ActionText->SetText(FText::FromString("OPEN"));
		break;

		// 추후 다른 타입도 확장 가능
	default:
		break;
	}

	// 공통 텍스트 처리 (유효할 때만)
	if (!InteractableData->Name.IsEmpty())
		NameText->SetText(InteractableData->Name);
	if (!InteractableData->Action.IsEmpty())
		ActionText->SetText(InteractableData->Action);
}

float UNS_InteractionWidget::UpdateInteractionProgress()
{
	return 0.0f;
}


