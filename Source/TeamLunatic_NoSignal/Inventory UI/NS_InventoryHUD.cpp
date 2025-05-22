// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/NS_InventoryHUD.h"
#include "Inventory UI/NS_InventoryMainMenu.h"
#include "Inventory UI/Interaction/NS_InteractionWidget.h"

ANS_InventoryHUD::ANS_InventoryHUD()
{
}

void ANS_InventoryHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryMainMenuClass)
	{
		InventoryMainMenuWidget = CreateWidget<UNS_InventoryMainMenu>(GetWorld(), InventoryMainMenuClass);
		InventoryMainMenuWidget->AddToViewport(5);
		InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UNS_InteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_InventoryHUD::DisplayMenu()
{
	if (InventoryMainMenuWidget)
	{
		bIsMenuVisible = true;
		InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ANS_InventoryHUD::HideMenu()
{
	if (InventoryMainMenuWidget)
	{
		bIsMenuVisible = false;
		InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_InventoryHUD::ToggleMenu()
{
	if (bIsMenuVisible)
	{
		HideMenu();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();

		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void ANS_InventoryHUD::ShowInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ANS_InventoryHUD::HideInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_InventoryHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}

