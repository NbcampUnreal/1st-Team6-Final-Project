// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/NS_QuickSlotComponent.h"
#include <Net/UnrealNetwork.h>
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "Inventory UI/Inventory/NS_QuickSlotSlotWidget.h"
#include "Engine/ActorChannel.h"

UNS_QuickSlotComponent::UNS_QuickSlotComponent()
{
	// 기본값으로 0번 슬롯 설정
	CurrentQuickSlotIndex = 0;
	
	// 복제 설정
	SetIsReplicatedByDefault(true);
	
	// 퀵슬롯 배열 초기화 (기본 5개 슬롯)
	QuickSlots.SetNum(5);
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		QuickSlots[i] = nullptr;
	}
}

void UNS_QuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	if (InitialSlotCount <= 0)
	{
		InitialSlotCount = 5;
	}

	QuickSlots.SetNum(InitialSlotCount);
	CleanInvalidSlots(); // 게임 시작 시 무효 슬롯 제거
}

// --- 슬롯 관리 ---

void UNS_QuickSlotComponent::AssignToSlot(int32 SlotIndex, UNS_InventoryBaseItem* Item)
{
	if (SlotIndex < 0 || SlotIndex >= QuickSlots.Num())
	{
		return;
	}
	
	QuickSlots[SlotIndex] = Item;
	
	// 현재 슬롯이 -1이면 이 슬롯으로 설정
	if (CurrentQuickSlotIndex < 0)
	{
		CurrentQuickSlotIndex = SlotIndex;
	}
	
	BroadcastSlotUpdate();
}

bool UNS_QuickSlotComponent::AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item)
{
	if (!Item) return false;
	
	// 이미 할당된 아이템인지 확인
	if (IsItemAlreadyAssigned(Item))
	{
		return false;
	}
	
	// 첫 번째 빈 슬롯 찾기
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (!QuickSlots[i])
		{
			QuickSlots[i] = Item;
			
			// 현재 슬롯이 -1이면 이 슬롯으로 설정
			if (CurrentQuickSlotIndex < 0)
			{
				CurrentQuickSlotIndex = i;
			}
			
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
			QuickSlots[i] = nullptr;
			bRemoved = true;
		}
	}

	if (bRemoved)
	{
		BroadcastSlotUpdate();
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

// 특정 아이템이 있는 슬롯 인덱스 찾기
int32 UNS_QuickSlotComponent::FindSlotIndexForItem(UNS_InventoryBaseItem* Item) const
{
	if (!Item)
	{
		return -1;
	}
	
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Item)
		{
			return i;
		}
	}
	
	return -1; // 아이템을 찾지 못함
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

// 현재 슬롯 인덱스 설정
void UNS_QuickSlotComponent::SetCurrentSlotIndex(int32 NewIndex)
{
	// 유효한 인덱스 범위 확인
	if (NewIndex < 0 || NewIndex >= QuickSlots.Num())
	{
		// 유효하지 않은 인덱스가 들어오면 0으로 설정
		if (NewIndex < 0)
		{
			NewIndex = 0;
		}
		else
		{
			NewIndex = QuickSlots.Num() - 1;
		}
	}
	
	CurrentQuickSlotIndex = NewIndex;
}

int32 UNS_QuickSlotComponent::GetTotalAssignedItems() const
{
    int32 Count = 0;
    for (const TObjectPtr<UNS_InventoryBaseItem>& SlotItem : QuickSlots)
    {
        if (SlotItem != nullptr)
        {
            Count++;
        }
    }
    return Count;
}
