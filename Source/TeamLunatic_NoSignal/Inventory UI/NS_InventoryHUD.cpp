// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/NS_InventoryHUD.h"
#include "Inventory UI/NS_InventoryMainMenu.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "Inventory UI/Interaction/NS_InteractionWidget.h"

// 생성자
ANS_InventoryHUD::ANS_InventoryHUD()
{
}

// 게임이 시작될 때 호출되는 함수
void ANS_InventoryHUD::BeginPlay()
{
	Super::BeginPlay();

	// 1. 데디서버에서는 UI 생성 금지
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	// 2. PlayerController 유효성 검사
	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	// 3. 인벤토리 메뉴 위젯 생성
	if (InventoryMainMenuClass)
	{
		InventoryMainMenuWidget = CreateWidget<UNS_InventoryMainMenu>(PC, InventoryMainMenuClass);
		if (InventoryMainMenuWidget)
		{
			InventoryMainMenuWidget->AddToViewport(5);
			InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//  4. 상호작용 위젯 생성
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UNS_InteractionWidget>(PC, InteractionWidgetClass);
		if (InteractionWidget)
		{
			InteractionWidget->AddToViewport(-1);
			InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

// 메뉴를 표시하는 함수
void ANS_InventoryHUD::DisplayMenu()
{
	if (InventoryMainMenuWidget)
	{
		bIsMenuVisible = true;
		InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

// 메뉴를 숨기는 함수
void ANS_InventoryHUD::HideMenu()
{
	if (InventoryMainMenuWidget)
	{
		bIsMenuVisible = false;
		InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// 인벤토리 위젯을 여는 함수
void ANS_InventoryHUD::OpenInventoryWidget()
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

// 상호작용 위젯을 표시하는 함수
void ANS_InventoryHUD::ShowInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

// 상호작용 위젯을 숨기는 함수
void ANS_InventoryHUD::HideInteractionWidget()
{
	if (!IsValid(InteractionWidget))
	{
		return;
	}

	InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}

// 상호작용 위젯을 업데이트하는 함수
void ANS_InventoryHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (!InteractionWidget)
	{
		return;
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

