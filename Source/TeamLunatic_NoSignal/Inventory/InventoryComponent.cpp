// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_PlayerHUD.h"
#include "UI/NS_UIManager.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Engine/ActorChannel.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	bWantsInitializeComponent = true;
}

// Subobject(UObject 기반 인벤토리 아이템) 복제 처리
bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UNS_InventoryBaseItem* Item : InventoryContents)
	{
		if (IsValid(Item))
		{
			bool bRep = Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			bWroteSomething |= bRep;

			if (bRep)
			{
				UE_LOG(LogTemp, Warning, TEXT("Replicated Inventory Item: %s"), *Item->GetName());
			}
		}
	}

	return bWroteSomething;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

// 서버와 클라이언트에게 인벤토리 UI 갱신 요청
void UInventoryComponent::BroadcastInventoryUpdate()
{
	UE_LOG(LogTemp, Warning, TEXT(" BroadcastInventoryUpdate() called"));
	OnInventoryUpdated.Broadcast(); // 서버용 UI 갱신

	// 클라이언트에게도 알림
	if (AController* Controller = Cast<AController>(GetOwner()->GetInstigatorController()))
	{
		if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(Controller->GetPawn()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling Client_NotifyInventoryUpdated()"));
			Player->Client_NotifyInventoryUpdated();
		}
	}
}

// 인벤토리 관련 변수 복제 설정
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryContents);
	DOREPLIFETIME(UInventoryComponent, InventoryTotalWeight);
}


//  인벤토리에 아이템을 추가하는 함수
FItemAddResult UInventoryComponent::HandleAddItem(UNS_InventoryBaseItem* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;
		
		// 스택 불가능한 아이템 처리
		if (!InputItem->NumericData.isStackable)
		{
			return HandleNonStackableItems(InputItem);
		}

		// 스택 가능한 아이템 처리
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		// 전량 추가된 경우
		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Successfully added{0} {1} to the Inventory."), InitialRequestedAddAmount, InputItem->TextData.ItemName));
		}
		// 일부만 추가된 경우
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Partial amount of {0} added to the Inventory. Number added = {1}"), InputItem->TextData.ItemName, StackableAmountAdded));
		}
		// 하나도 추가되지 못한 경우
		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(FText::FromString("Could not add{0} to the Inventory. No remaining Inventory slots, or invalid Item."), InputItem->TextData.ItemName));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));
}

// 인벤토리에 동일한 아이템이 있는지 확인
UNS_InventoryBaseItem* UInventoryComponent::FindMatchingItem(UNS_InventoryBaseItem* ItemIn) const
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

// 동일한 ID를 가진 다음 아이템을 찾음
UNS_InventoryBaseItem* UInventoryComponent::FindNextItemByID(UNS_InventoryBaseItem* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UNS_InventoryBaseItem>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

// 스택이 가득 차지 않은 아이템 중 동일한 아이템 찾기
UNS_InventoryBaseItem* UInventoryComponent::FindNextPartialStack(UNS_InventoryBaseItem* ItemIn) const
{
	if (const TArray<TObjectPtr<UNS_InventoryBaseItem>>::ElementType* Result = InventoryContents.FindByPredicate([&ItemIn](const UNS_InventoryBaseItem* InventoryItem)
		{
			return InventoryItem->ItemDataRowName == ItemIn->ItemDataRowName && !InventoryItem->IsFullItemStack();
		}
	))
	{
		return *Result;
	}

	return nullptr;
}

// 인벤토리에서 특정 아이템을 하나 제거
void UInventoryComponent::RemoveSingleInstanceOfItem(UNS_InventoryBaseItem* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);

	if (AActor* Owner = GetOwner())
	{
		if (UNS_QuickSlotComponent* QSlotComp = Owner->FindComponentByClass<UNS_QuickSlotComponent>())
		{
			QSlotComp->RemoveItem(ItemToRemove); // 퀵슬롯에서 제거도 처리
		}
	}

	BroadcastInventoryUpdate();
}

// 특정 수량만큼 아이템 제거
int32 UInventoryComponent::RemoveAmountOfItem(UNS_InventoryBaseItem* ItemIn, int32 DesiredAmountToRemove)
{
	if (!ItemIn || DesiredAmountToRemove <= 0)
		return 0;

	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);
	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();

	if (ItemIn->Quantity <= 0)
	{
		RemoveSingleInstanceOfItem(ItemIn);
	}

	BroadcastInventoryUpdate();

	return ActualAmountToRemove;
}

// 기존 스택을 분할해서 새로운 스택 생성
void UInventoryComponent::SplitExistingStack(UNS_InventoryBaseItem* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

// 스택 불가능한 아이템 처리
FItemAddResult UInventoryComponent::HandleNonStackableItems(UNS_InventoryBaseItem* InputItem)
{
	// 유효하지 않은 무게
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. Item has invalid weight value."), InputItem->TextData.ItemName));
	}
	// 무게 초과
	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. Item would overflow weight limit."), InputItem->TextData.ItemName));
	}
	// 슬롯 초과
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add{0} to the Inventory. All Inventory slots are full."), InputItem->TextData.ItemName));
	}
	// 새 아이템 추가
	AddNewItem(InputItem, 1);
	return FItemAddResult::AddedAll(1, FText::Format(FText::FromString("Successfully added a single {0} to the Inventory."), InputItem->TextData.ItemName));
}

// 스택 가능한 아이템 처리
int32 UInventoryComponent::HandleStackableItems(UNS_InventoryBaseItem* ItemIn, int32 RequestedAddAmount)
{
	if (!IsValid(ItemIn) || RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		UE_LOG(LogTemp, Error, TEXT("[HandleStackableItems] ItemIn이 유효하지 않거나 수량/무게가 0입니다."));
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;
	// 1. 기존 스택에 추가 시도
	UNS_InventoryBaseItem* ExstingItemStack = FindNextPartialStack(ItemIn);

	while (ExstingItemStack)
	{
		if (!IsValid(ExstingItemStack))
		{
			UE_LOG(LogTemp, Error, TEXT("[HandleStackableItems] ExstingItemStack가 nullptr입니다. 중단."));
			break;
		}
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExstingItemStack, AmountToDistribute);
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExstingItemStack, AmountToMakeFullStack);

		if (WeightLimitAddAmount > 0)
		{
			ExstingItemStack->SetQuantity(ExstingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += (ExstingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);

			AmountToDistribute -= WeightLimitAddAmount;

			ItemIn->SetQuantity(AmountToDistribute);

			if (InventoryTotalWeight + ExstingItemStack->GetItemSingleWeight()>InventoryWeightCapacity)
			{
				BroadcastInventoryUpdate();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				BroadcastInventoryUpdate();
				return RequestedAddAmount - AmountToDistribute;
			}

			return 0;
		}

		if (AmountToDistribute <= 0)
		{
			BroadcastInventoryUpdate();
			return RequestedAddAmount;
		}

		ExstingItemStack = FindNextPartialStack(ItemIn);
	}
	// 2. 새 스택 생성 시도
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				UNS_InventoryBaseItem* NewItemCopy = ItemIn->CreateItemCopy();
				if (!IsValid(NewItemCopy))
				{
					UE_LOG(LogTemp, Error, TEXT("[HandleStackableItems] CreateItemCopy() 실패."));
					return RequestedAddAmount - AmountToDistribute;
				}

				AddNewItem(NewItemCopy, WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}
	}

	BroadcastInventoryUpdate();
	return RequestedAddAmount - AmountToDistribute;
}

// 무게 제한을 고려한 수량 계산
int32 UInventoryComponent::CalculateWeightAddAmount(UNS_InventoryBaseItem* ItemIn, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

// 스택을 가득 채우기 위해 필요한 수량 계산
int32 UInventoryComponent::CalculateNumberForFullStack(UNS_InventoryBaseItem* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStack - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

//  새 아이템 인벤토리에 추가
void UInventoryComponent::AddNewItem(UNS_InventoryBaseItem* Item, const int32 AmountToAdd)
{
	if (!Item)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] AddNewItem 실패 - Item == nullptr"));
		return;
	}

	if (InventoryContents.Num() >= 20)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] 슬롯 초과 - 현재: %d / 20"), InventoryContents.Num());
		return;
	}

	// 임시 무게 확인
	float IncomingWeight = Item->GetItemStackWeight();
	if (InventoryTotalWeight + IncomingWeight > GetWeightCapacity())
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] 무게 초과: %.1f + %.1f > %.1f"), InventoryTotalWeight, IncomingWeight, GetWeightCapacity());
		return;
	}
	UNS_InventoryBaseItem* NewItem;

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
	BroadcastInventoryUpdate();
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] Added %s"), *NewItem->GetName());
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] NewItem OwingInventory: %s"), *GetNameSafe(NewItem->OwingInventory));

	// 쪽지 아이템 획득 시 PlayerHUD의 TipText 숨기기 처리 (클라이언트 RPC 사용)
    	if (NewItem->ItemType == EItemType::Misc)
    	{
    		AActor* OwnerActor = GetOwner();
    		if (OwnerActor)
    		{
    			ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OwnerActor);
    			if (Player)
    			{
    				// 클라이언트 RPC를 통해 TipText 숨기기 처리
    				Player->Multicast_HideTipText();
    				UE_LOG(LogTemp, Warning, TEXT("AddNewItem: Misc 아이템 획득으로 Client_HideTipText() 호출"));
    			}
    		}
    	}
}

void UInventoryComponent::CleanUpZeroQuantityItems()
{
	int32 BeforeNum = InventoryContents.Num();
	float WeightToRemove = 0.f;

	// 제거 대상 아이템을 따로 모아서 무게 합산
	TArray<UNS_InventoryBaseItem*> ItemsToRemove;
	for (UNS_InventoryBaseItem* Item : InventoryContents)
	{
		if (!IsValid(Item) || Item->Quantity <= 0)
		{
			if (IsValid(Item))
			{
				WeightToRemove += Item->GetItemStackWeight(); // 스택 전체 무게 합산
			}
			ItemsToRemove.Add(Item);
		}
	}

	// 실제 제거
	for (UNS_InventoryBaseItem* Item : ItemsToRemove)
	{
		InventoryContents.RemoveSingle(Item);
	}

	if (ItemsToRemove.Num() > 0)
	{
		InventoryTotalWeight -= WeightToRemove;
		InventoryTotalWeight = FMath::Max(0.f, InventoryTotalWeight); // 음수 방지

		BroadcastInventoryUpdate();
	}
}

// 해당 무기 유형에 맞는 탄약이 인벤토리에 존재하는지 확인하는 함수
bool UInventoryComponent::HasAmmoForWeapon(EAmmoType WeaponAmmoType) const
{
	// 인벤토리에 있는 모든 아이템을 조회
	for (const auto& Item : InventoryContents)
	{
		// 아이템이 유효하고,
		// 아이템 타입이 '장비'이며,
		// 무기 타입이 Ammo이고, 
		// 아이템의 탄약 타입이 무기가 요구하는 탄약 타입과 일치하고,
		// 수량이 1개 이상인 경우
		if (Item &&
			Item->ItemType == EItemType::Equipment &&
			Item->WeaponType == EWeaponType::Ammo &&
			Item->WeaponData.AmmoType == WeaponAmmoType &&
			Item->Quantity > 0)
		{
			// 조건에 맞는 탄약을 찾았으므로 true 반환
			return true;
		}
	}
	// 조건을 만족하는 탄약을 찾지 못했으므로 false 반환
	return false;
}


