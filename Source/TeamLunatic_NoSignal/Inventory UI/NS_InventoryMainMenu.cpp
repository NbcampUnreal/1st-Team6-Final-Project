// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/NS_InventoryMainMenu.h"
#include "Inventory UI/Inventory/ItemDragDropOperation.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseItem.h"

void UNS_InventoryMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UNS_InventoryMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn());
}

bool UNS_InventoryMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;
}
