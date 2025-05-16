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
	
	case EInteractableType::NonPlayerCharacter:
		break;
	
	case EInteractableType::Device:
		break;
	
	case EInteractableType::Toggle:
		break;
	
	case EInteractableType::Container:
		break;
	
	default:;
	}
	ActionText->SetText(InteractableData->Action);
	NameText->SetText(InteractableData->Name);
}

float UNS_InteractionWidget::UpdateInteractionProgress()
{
	return 0.0f;
}


