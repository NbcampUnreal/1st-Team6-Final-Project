#include "Inventory UI/NS_InventoryHUD.h"
#include "Inventory UI/NS_InventoryMainMenu.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "Inventory UI/Interaction/NS_InteractionWidget.h"
#include "Inventory UI/NS_NearbyItemsWidget.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "Inventory UI/NS_PlayerWidget.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"

ANS_InventoryHUD::ANS_InventoryHUD()
{
}

void ANS_InventoryHUD::BeginPlay()
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

	// 인벤토리 메뉴 위젯 생성
	if (InventoryMainMenuClass)
	{
		InventoryMainMenuWidget = CreateWidget<UNS_InventoryMainMenu>(PC, InventoryMainMenuClass);
		if (InventoryMainMenuWidget)
		{
			InventoryMainMenuWidget->AddToViewport(5);
			InventoryMainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 상호작용 위젯 생성
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UNS_InteractionWidget>(PC, InteractionWidgetClass);
		if (InteractionWidget)
		{
			InteractionWidget->AddToViewport(-1);
			InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	// 주변 아이템 위젯 생성
	if (NearbyItemsWidgetClass)
	{
		NearbyItemsWidget = CreateWidget<UNS_NearbyItemsWidget>(PC, NearbyItemsWidgetClass);
		if (NearbyItemsWidget)
		{
			NearbyItemsWidget->AddToViewport(6);
			NearbyItemsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
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

void ANS_InventoryHUD::OpenInventoryWidget()
{
	if (bIsMenuVisible)
	{
		HideMenu();
		HideNearbyItemsWidget();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();
		ShowNearbyItemsWidget();

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

void ANS_InventoryHUD::ShowNearbyItemsWidget()
{
	if (NearbyItemsWidget)
	{
		NearbyItemsWidget->SetPositionInViewport(FVector2D(50, 50));
		NearbyItemsWidget->SetDesiredSizeInViewport(FVector2D(300, 400));
		NearbyItemsWidget->SetVisibility(ESlateVisibility::Visible);
		
		if (!NearbyItemsWidget->IsInViewport())
		{
			NearbyItemsWidget->AddToViewport(100);
		}
		
		if (InteractionComponent)
		{
			UpdateNearbyItemsWidget(InteractionComponent->GetNearbyItems());
		}
	}
}

void ANS_InventoryHUD::HideNearbyItemsWidget()
{
	if (NearbyItemsWidget)
	{
		NearbyItemsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ANS_InventoryHUD::UpdateNearbyItemsWidget(const TArray<FNearbyItemInfo>& NearbyItems)
{
	if (NearbyItemsWidget && NearbyItemsWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		NearbyItemsWidget->UpdateItemsList(NearbyItems);
	}
}

void ANS_InventoryHUD::SetInteractionComponent(UNS_InteractionComponent* InInteractionComponent)
{
	InteractionComponent = InInteractionComponent;
	
	if (InteractionComponent)
	{
		InteractionComponent->OnNearbyItemsUpdated.AddDynamic(this, &ANS_InventoryHUD::OnNearbyItemsUpdated);
	}
}

void ANS_InventoryHUD::OnNearbyItemsUpdated()
{
	if (InteractionComponent && bIsMenuVisible)
	{
		UpdateNearbyItemsWidget(InteractionComponent->GetNearbyItems());
	}
}

void ANS_InventoryHUD::UpdatePlayerHealth(int32 CurrentHealth, int32 MaxHealth)
{
	if (PlayerWidget)
	{
		PlayerWidget->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void ANS_InventoryHUD::UpdatePlayerStamina(int32 CurrentStamina, int32 MaxStamina)
{
	if (PlayerWidget)
	{
		PlayerWidget->UpdateStamina(CurrentStamina, MaxStamina);
	}
}

void ANS_InventoryHUD::SetCrosshairVisibility(bool bVisible)
{
	if (PlayerWidget)
	{
		PlayerWidget->SetCrosshairVisibility(bVisible);
	}
}

void ANS_InventoryHUD::SetPlayerCharacter(ANS_PlayerCharacterBase* InPlayerCharacter)
{
	PlayerCharacter = InPlayerCharacter;
	
	if (PlayerCharacter && PlayerCharacter->StatusComp)
	{
		UpdatePlayerHealth(PlayerCharacter->StatusComp->Health, PlayerCharacter->StatusComp->MaxHealth);
		UpdatePlayerStamina(PlayerCharacter->StatusComp->Stamina, PlayerCharacter->StatusComp->MaxStamina);
		
		PlayerCharacter->StatusComp->OnHealthChanged.AddDynamic(this, &ANS_InventoryHUD::UpdatePlayerHealth);
		PlayerCharacter->StatusComp->OnStaminaChanged.AddDynamic(this, &ANS_InventoryHUD::UpdatePlayerStamina);
	}
}