// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include <Net/UnrealNetwork.h>
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Inventory UI/Inventory/NS_QuickSlotSlotWidget.h"

#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UNS_QuickSlotComponent::UNS_QuickSlotComponent()
{
	SetIsReplicatedByDefault(true);
}

void UNS_QuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	QuickSlots.SetNum(InitialSlotCount);
	CleanInvalidSlots(); // 게임 시작 시 무효 슬롯 제거
}

// --- 슬롯 관리 ---

void UNS_QuickSlotComponent::AssignToSlot(int32 Index, UNS_InventoryBaseItem* Item)
{
	if (!Item || !QuickSlots.IsValidIndex(Index)) return;
	QuickSlots[Index] = Item;
	BroadcastSlotUpdate();
}

bool UNS_QuickSlotComponent::AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item)
{
	if (!Item || IsItemAlreadyAssigned(Item)) return false;
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (!QuickSlots[i]) {
			QuickSlots[i] = Item;
			BroadcastSlotUpdate();
			return true;
		}
	}
	return false;
}

bool UNS_QuickSlotComponent::RemoveItem(UNS_InventoryBaseItem* Item)
{
	if (!Item) return false;
	bool bRemoved = false;

	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 슬롯 %d에서 아이템 제거됨: %s"), i, *Item->GetName());
			QuickSlots[i] = nullptr;
			bRemoved = true;
		}
		else if (QuickSlots[i])
		{
			UE_LOG(LogTemp, Verbose, TEXT("[QuickSlot] 슬롯 %d: 다른 아이템 %s 존재"), i, *QuickSlots[i]->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("[QuickSlot] 슬롯 %d: 비어 있음"), i);
		}
	}

	if (bRemoved)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 아이템 제거 후 슬롯 상태 갱신됨"));
		BroadcastSlotUpdate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 대상 아이템을 슬롯에서 찾지 못함: %s"), *GetNameSafe(Item));
	}

	return bRemoved;
}

void UNS_QuickSlotComponent::CleanInvalidSlots()
{
	bool bChanged = false;

	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] && !IsValid(QuickSlots[i])) // 아이템이 존재하지만 무효일 때만 제거
		{
			QuickSlots[i] = nullptr;
			bChanged = true;
		}
	}

	if (bChanged)
	{
		BroadcastSlotUpdate();
	}
}

// --- 조회 ---

UNS_InventoryBaseItem* UNS_QuickSlotComponent::GetItemInSlot(int32 Index) const
{
	return QuickSlots.IsValidIndex(Index) ? QuickSlots[Index] : nullptr;
}

bool UNS_QuickSlotComponent::IsItemAlreadyAssigned(UNS_InventoryBaseItem* Item) const
{
	return QuickSlots.Contains(Item);
}

// --- 복제 및 UI 연동 ---

void UNS_QuickSlotComponent::BroadcastSlotUpdate()
{
	QuickSlotUpdated.Broadcast();
}

void UNS_QuickSlotComponent::OnRep_QuickSlots()
{
	CleanInvalidSlots();
	BroadcastSlotUpdate();
}

bool UNS_QuickSlotComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (UNS_InventoryBaseItem* Item : QuickSlots)
	{
		if (Item)
		{
			bWrote |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
	}
	return bWrote;
}

void UNS_QuickSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UNS_QuickSlotComponent, QuickSlots);
}
