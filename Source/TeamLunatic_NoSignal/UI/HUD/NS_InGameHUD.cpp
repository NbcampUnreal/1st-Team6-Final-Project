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
			PlayerWidget->AddToViewport();
			PlayerWidget->SetVisibility(ESlateVisibility::Visible);
			// 초기 활성화 위젯을 플레이어 위젯으로 설정
			CurrentWidget = PlayerWidget;
		}
	}
	
	// 인벤토리 메뉴 위젯 생성
	if (InventoryMainClass)
	{
		InventoryMainWidget = CreateWidget<UNS_InventoryMainWidget>(PC, InventoryMainClass);
		if (InventoryMainWidget)
		{
			InventoryMainWidget->AddToViewport();
			InventoryMainWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 레벨지도 위젯 생성
	if (LevelMapWidgetClass)
	{
		LevelMapWidget = CreateWidget<UNS_LevelMapWidget>(PC, LevelMapWidgetClass);
		if (LevelMapWidget)
		{
			LevelMapWidget->AddToViewport();
			LevelMapWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// ESC 위젯 생성
	if (ESCWidgetClass)
	{
		ESCWidget = CreateWidget<UUserWidget>(PC, ESCWidgetClass);
		if (ESCWidget)
		{
			ESCWidget->AddToViewport();
			ESCWidget->SetVisibility(ESlateVisibility::Collapsed);
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
		if (ESCWidget)
		{
			ESCWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 현재 활성화된 위젯 업데이트
	CurrentWidget = OpenWidget;

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
	else // ESC 위젯 또는 기타 위젯인 경우
	{
		if (OpenWidget == ESCWidget)
		{
			ESCWidget->SetVisibility(ESlateVisibility::Visible);
			PC->SetInputMode(FInputModeGameAndUI());
			PC->SetShowMouseCursor(true);
		}
	}
}