// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Item/NS_BaseWeapon.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

FItemAddResult UInventoryComponent::HandleAddItem(ANS_BaseWeapon* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		if (!InputItem->ItemNumericData.isStackable)
		{
			return HandleNonStackableItems(InputItem, InitialRequestedAddAmount);
		}

		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Successfully added{0} {1} to the Inventory."), InitialRequestedAddAmount, InputItem->ItemTextData.ItemName));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Partial amount of {0} added to the Inventory. Number added = {1}"), InputItem->ItemTextData.ItemName, StackableAmountAdded));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Could not add{0} to the Inventory. No remaining Inventory slots, or invalid Item."), InputItem->ItemTextData.ItemName));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));
}

ANS_BaseWeapon* UInventoryComponent::FindMatchingItem(ANS_BaseWeapon* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

ANS_BaseWeapon* UInventoryComponent::FindNextItemByID(ANS_BaseWeapon* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<ANS_BaseWeapon>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

ANS_BaseWeapon* UInventoryComponent::FindNextPartialStack(ANS_BaseWeapon* ItemIn) const
{
	if (const TArray<TObjectPtr<ANS_BaseWeapon>>::ElementType* Result = InventoryContents.FindByPredicate([&ItemIn](const ANS_BaseWeapon* InventoryItem)
		{
			return InventoryItem->WeaponDataRowName == ItemIn->WeaponDataRowName && !InventoryItem->IsFullItemStack();
		}
	))
	{
		return *Result;
	}

	return nullptr;
}

void UInventoryComponent::RemoveSingleInstanceOfItem(ANS_BaseWeapon* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(ANS_BaseWeapon* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(ANS_BaseWeapon* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(ANS_BaseWeapon* InputItem, int32 RequestedAddAmount)
{
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. Item has invalid weight value."), InputItem->ItemTextData.ItemName));
	}

	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. Item would overflow weight limit."), InputItem->ItemTextData.ItemName));
	}

	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. All Inventory slots are full."), InputItem->ItemTextData.ItemName));
	}

	AddNewItem(InputItem, RequestedAddAmount);
	return FItemAddResult::AddedAll(RequestedAddAmount, FText::Format(FText::FromString("Successfully added{0} {1} to the Inventory."), RequestedAddAmount, InputItem->ItemTextData.ItemName));
}

int32 UInventoryComponent::HandleStackableItems(ANS_BaseWeapon* ItemIn, int32 RequestedAddAmount)
{
	return int32();
}

int32 UInventoryComponent::CalculateWeightAddAmount(ANS_BaseWeapon* ItemIn, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(ANS_BaseWeapon* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->ItemNumericData.MaxStack - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::AddNewItem(ANS_BaseWeapon* Item, const int32 AmountToAdd)
{
	ANS_BaseWeapon* NewItem;

	if (Item->bisCopy || Item->bisPickup)
	{
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwingInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}


