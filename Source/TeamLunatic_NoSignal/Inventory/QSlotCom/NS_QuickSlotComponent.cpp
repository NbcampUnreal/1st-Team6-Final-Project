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

	UE_LOG(LogTemp, Warning, TEXT("[QuickSlotComponent] 생성자 - 기본 슬롯 인덱스: %d, 슬롯 수: %d"),
		CurrentQuickSlotIndex, QuickSlots.Num());
}

void UNS_QuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[QuickSlotComponent] BeginPlay 실행됨"));

	if (InitialSlotCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlotComponent] InitialSlotCount 값이 0 또는 미설정! 기본값 5로 대체"));
		InitialSlotCount = 5;
	}

	QuickSlots.SetNum(InitialSlotCount);
	UE_LOG(LogTemp, Warning, TEXT("[QuickSlotComponent] 슬롯 초기화: %d개"), InitialSlotCount);
	CleanInvalidSlots(); // 게임 시작 시 무효 슬롯 제거
}

// --- 슬롯 관리 ---

void UNS_QuickSlotComponent::AssignToSlot(int32 SlotIndex, UNS_InventoryBaseItem* Item)
{
	if (SlotIndex < 0 || SlotIndex >= QuickSlots.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("[QuickSlot] 유효하지 않은 슬롯 인덱스: %d"), SlotIndex);
		return;
	}

	QuickSlots[SlotIndex] = Item;

	// 현재 슬롯이 -1이면 이 슬롯으로 설정
	if (CurrentQuickSlotIndex < 0)
	{
		CurrentQuickSlotIndex = SlotIndex;
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 현재 슬롯 인덱스 업데이트: %d"), CurrentQuickSlotIndex);
	}

	UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 아이템 할당됨 - 슬롯: %d, 아이템: %s"),
		SlotIndex, Item ? *Item->GetName() : TEXT("nullptr"));

	BroadcastSlotUpdate();
}

bool UNS_QuickSlotComponent::AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item)
{
	if (!Item) return false;

	// 이미 할당된 아이템인지 확인
	if (IsItemAlreadyAssigned(Item))
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 이미 할당된 아이템: %s"), *Item->GetName());
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
				UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 현재 슬롯 인덱스 업데이트: %d"), CurrentQuickSlotIndex);
			}

			UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 아이템 할당됨 - 슬롯: %d, 아이템: %s"), i, *Item->GetName());
			BroadcastSlotUpdate();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 빈 슬롯 없음 - 아이템 할당 실패: %s"), *Item->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 유효하지 않은 슬롯 인덱스 설정 시도: %d, 범위: 0-%d"),
			NewIndex, QuickSlots.Num() - 1);

		// 유효하지 않은 인덱스가 들어오면 0으로 설정
		if (NewIndex < 0)
		{
			NewIndex = 0;
			UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 인덱스가 음수여서 0으로 설정"));
		}
		else
		{
			NewIndex = QuickSlots.Num() - 1;
			UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 인덱스가 범위를 초과하여 최대값으로 설정"));
		}
	}

	CurrentQuickSlotIndex = NewIndex;
	UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] 현재 슬롯 인덱스 설정: %d"), CurrentQuickSlotIndex);
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