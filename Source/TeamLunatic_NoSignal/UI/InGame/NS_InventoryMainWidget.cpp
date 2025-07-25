#include "UI/InGame/NS_InventoryMainWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/InGame/NS_QuickSlotPanel.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Character/Components/NS_QuickSlotComponent.h"

void UNS_InventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (QuickSlotComponent)
	{
		QuickSlotComponent->QuickSlotUpdated.AddUObject(this, &UNS_InventoryMainWidget::OnQuickSlotUpdated);
		// Initialize
		OnQuickSlotUpdated();
	}
}

void UNS_InventoryMainWidget::OnQuickSlotUpdated()
{
	if (QuickSlotPanel && QuickSlotComponent)
	{
		QuickSlotPanel->RefreshQuickSlots(QuickSlotComponent->GetQuickSlots());
	}
}

