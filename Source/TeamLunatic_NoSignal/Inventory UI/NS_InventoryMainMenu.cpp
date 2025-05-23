// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/NS_InventoryMainMenu.h"

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
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// cast operation to Item drag drop, ensure player is valid, call drop Item on player
}
