#include "UI/HUD/NS_InGameHUD.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "UI/InGame/NS_InventoryMainWidget.h"
#include "UI/InGame/NS_InteractionPanel.h"
#include "UI/InGame/NS_PlayerWidget.h"
#include "UI/INGame/NS_LevelMapWidget.h"

void ANS_InGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	// 플레이어 위젯 생성
	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UNS_PlayerWidget>(PC, PlayerWidgetClass);
		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport(1);
			PlayerWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	// 인벤토리 메뉴 위젯 생성
	if (InventoryMainClass)
	{
		InventoryMainWidget = CreateWidget<UNS_InventoryMainWidget>(PC, InventoryMainClass);
		if (InventoryMainWidget)
		{
			InventoryMainWidget->AddToViewport(5);
			InventoryMainWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 레벨지도 위젯 생성
	if (LevelMapWidgetClass)
	{
		LevelMapWidget = CreateWidget<UNS_LevelMapWidget>(PC, LevelMapWidgetClass);
		if (LevelMapWidget)
		{
			LevelMapWidget->AddToViewport(2);
			LevelMapWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ANS_InGameHUD::ShowWidget(UUserWidget* OpenWidget)
{
	if (!OpenWidget)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) return;
	
	if (OpenWidget)
	{
		if (PlayerWidget)
		{
			PlayerWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (InventoryMainWidget)
		{
			InventoryMainWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (LevelMapWidget)
		{
			LevelMapWidget->SetVisibility(ESlateVisibility::Collapsed);
		}	
	}

	if (OpenWidget->IsA(UNS_PlayerWidget::StaticClass()))
	{
		if (PlayerWidget)
		{
			PlayerWidget->SetVisibility(ESlateVisibility::Visible);
			PC->SetInputMode(FInputModeGameOnly());
			PC->SetShowMouseCursor(false);

		}
	}
	else if (OpenWidget->IsA(UNS_InventoryMainWidget::StaticClass()))
	{
		if (InventoryMainWidget)
		{
			InventoryMainWidget->SetVisibility(ESlateVisibility::Visible);
			PC->SetInputMode(FInputModeGameAndUI());
			PC->SetShowMouseCursor(true);
		}
	}
	else if (OpenWidget->IsA(UNS_LevelMapWidget::StaticClass()))
	{
		if (LevelMapWidget)
		{
			LevelMapWidget->SetVisibility(ESlateVisibility::Visible);
			PC->SetInputMode(FInputModeGameAndUI());
			PC->SetShowMouseCursor(true);
		}
	}
}