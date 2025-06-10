// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/NS_InventoryHUD.h"
#include "Inventory UI/NS_InventoryMainMenu.h"
#include "Interaction/InteractionInterface.h"
#include "Inventory UI/Interaction/NS_InteractionWidget.h"

ANS_InventoryHUD::ANS_InventoryHUD()
{
}

void ANS_InventoryHUD::BeginPlay()
{
	Super::BeginPlay();

	// 1. 데디서버에서는 UI 생성 금지
	if (GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dedicated Server - HUD 초기화 중단"));
		return;
	}

	// 2. PlayerController 유효성 검사
	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null - HUD 초기화 실패"));
		return;
	}

	// 3. 인벤토리 메뉴 위젯 생성
	if (!InventoryMainMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryMainMenuClass가 설정되지 않았습니다."));
	}
	else
	{
		InventoryMainMenuWidget = CreateWidget<UNS_InventoryMainMenu>(PC, InventoryMainMenuClass);
		if (InventoryMainMenuWidget)
		{
			InventoryMainMenuWidget->AddToViewport(5);
			InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryMainMenuWidget 생성 실패"));
		}
	}

	//  4. 상호작용 위젯 생성
	if (!InteractionWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionWidgetClass가 설정되지 않았습니다. HUD BP 확인 요망."));
	}
	else
	{
		InteractionWidget = CreateWidget<UNS_InteractionWidget>(PC, InteractionWidgetClass);
		if (!InteractionWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("InteractionWidget 생성 실패 - 위젯 클래스 확인 필요"));
		}
		else
		{
			InteractionWidget->AddToViewport(-1);
			InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Warning, TEXT("InteractionWidget 생성 완료"));
		}
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
	if (!IsValid(InteractionWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("[HUD] InteractionWidget이 유효하지 않습니다."));
		return;
	}

	InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogTemp, Warning, TEXT("[HUD] Interaction 위젯 숨김 처리 완료"));
}

void ANS_InventoryHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (!InteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateInteractionWidget: InteractionWidget가 nullptr입니다."));
		return;
	}

	// 상호작용 타입 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("UpdateInteractionWidget - Type: %d, Name: %s, Action: %s"),
		static_cast<uint8>(InteractableData->InteractableType),
		*InteractableData->Name.ToString(),
		*InteractableData->Action.ToString());

	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}

