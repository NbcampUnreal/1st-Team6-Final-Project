#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseWeapon.h"
#include "GameFlow/NS_GameState.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseMagazine.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "Character/NS_PlayerController.h"
#include "UI/NS_PlayerHUD.h"
#include "Net/UnrealNetwork.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitializePickup(UNS_InventoryBaseItem::StaticClass(), ItemQuantity);
	}
	else
	{
		OnRep_ReplicatedItemData();
	}
}

void APickup::InitializePickup(const TSubclassOf<UNS_InventoryBaseItem> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString());

		ReplicatedItemData = *ItemData;
		ReplicatedItemData.Quantity = InQuantity > 0 ? InQuantity : 1;
		OnRep_ReplicatedItemData();

		ItemReference = NewObject<UNS_InventoryBaseItem>(GetWorld(), BaseClass);
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->WeaponType = ItemData->WeaponType;
		ItemReference->WeaponData = ItemData->WeaponData;
		ItemReference->TextData = ItemData->ItemTextData;
		ItemReference->NumericData = ItemData->ItemNumericData;
		ItemReference->AssetData = ItemData->ItemAssetData;
		ItemReference->ItemDataRowName = ItemData->ItemDataRowName;
		ItemReference->CurrentAmmo = ItemData->CurrentAmmo;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UNS_InventoryBaseItem* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.StaticMesh);

	ReplicatedItemData.ItemDataRowName = ItemToDrop->ItemDataRowName;
	ReplicatedItemData.ItemTextData = ItemToDrop->TextData;
	ReplicatedItemData.ItemNumericData = ItemToDrop->NumericData;
	ReplicatedItemData.ItemAssetData = ItemToDrop->AssetData;
	ReplicatedItemData.WeaponData = ItemToDrop->WeaponData;
	ReplicatedItemData.WeaponType = ItemToDrop->WeaponType;
	ReplicatedItemData.ItemType = ItemToDrop->ItemType;
	ReplicatedItemData.CurrentAmmo = ItemToDrop->CurrentAmmo;

	UpdateInteractableData();
}

void APickup::OnRep_ReplicatedItemData()
{
	if (!ItemReference)
	{
		ItemReference = NewObject<UNS_InventoryBaseItem>(GetWorld(), UNS_InventoryBaseItem::StaticClass());
	}

	ItemReference->ItemDataRowName = ReplicatedItemData.ItemDataRowName;
	ItemReference->ItemsDataTable = ItemDataTable;
	ItemReference->TextData = ReplicatedItemData.ItemTextData;
	ItemReference->NumericData = ReplicatedItemData.ItemNumericData;
	ItemReference->AssetData = ReplicatedItemData.ItemAssetData;
	ItemReference->WeaponData = ReplicatedItemData.WeaponData;
	ItemReference->WeaponType = ReplicatedItemData.WeaponType;
	ItemReference->ItemType = ReplicatedItemData.ItemType;
	ItemReference->CurrentAmmo = ReplicatedItemData.CurrentAmmo;

	PickupMesh->SetStaticMesh(ReplicatedItemData.ItemAssetData.StaticMesh);
	ItemReference->SetQuantity(ReplicatedItemData.Quantity);

	UpdateInteractableData();
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.ItemName;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact_Implementation(AActor* InteractingActor)
{
	if (!HasAuthority())
	{
		Server_TakePickup(InteractingActor);
		return;
	}

	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(ANS_PlayerCharacterBase* Taker)
{
	if (!HasAuthority()) return;

	// IsPickUp 상태 체크 - 이미 아이템 획득 중이면 추가 획득 차단
	if (Taker->IsPickUp)
	{
		UE_LOG(LogTemp, Warning, TEXT("TakePickup: 이미 아이템 획득 중 - 추가 획득 차단 (IsPickUp = true)"));
		return;
	}

    Taker->IsPickUp = true;

	if (!IsPendingKillPending())
	{
		if (!ItemReference || !IsValid(ItemReference))
		{
			return;
		}
		if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
		{
			// 인벤토리에 아이템 추가 시도
			const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

			// 아이템이 성공적으로 (전부 또는 부분적으로) 추가되었다면
			if (AddResult.OperationResult == EItemAddResult::TAR_AllItemAdded || AddResult.OperationResult == EItemAddResult::TAR_PartialAmountItemAdded)
			{
                if (ItemReference && ItemReference->AssetData.GetSound)
                {
                    Taker->Multicast_PlayPickupSound(ItemReference->AssetData.GetSound);
                }
				// 아이템 추가 성공 시 장비 아이템인 경우 자동 퀵슬롯 할당 처리
				if (AddResult.ActualAmountAdded > 0 &&
					ItemReference->ItemType == EItemType::Equipment &&
					ItemReference->WeaponType != EWeaponType::Ammo)
				{
					// 퀵슬롯 컴포넌트 확인
					if (UNS_QuickSlotComponent* QuickSlotComp = Taker->GetQuickSlotComponent())
					{
						// 인벤토리에 추가된 아이템 찾기
						UNS_InventoryBaseItem* AddedItem = nullptr;

						// 인벤토리에서 같은 ItemDataRowName을 가진 아이템 찾기
						for (UNS_InventoryBaseItem* Item : PlayerInventory->GetInventoryContents())
						{
							if (Item && Item->ItemDataRowName == ItemReference->ItemDataRowName)
							{
								AddedItem = Item;
								break;
							}
						}

						if (AddedItem)
						{
							// 현재 선택된 퀵슬롯 인덱스 가져오기
							int32 CurrentSlotIndex = Taker->GetCurrentQuickSlotIndex();
							bool bAssignedToCurrentSlot = false;

							// 아이템이 이미 퀵슬롯에 할당되어 있는지 확인
							if (!QuickSlotComp->IsItemAlreadyAssigned(AddedItem))
							{
								int32 AssignedSlot = -1;

								// 현재 선택된 슬롯이 비어있으면 해당 슬롯에 할당
								if (!QuickSlotComp->GetItemInSlot(CurrentSlotIndex))
								{
									QuickSlotComp->AssignToSlot(CurrentSlotIndex, AddedItem);
									AssignedSlot = CurrentSlotIndex;
									bAssignedToCurrentSlot = true;
								}
								else
								{
									// 현재 선택된 슬롯이 이미 차있으면 첫 번째 빈 슬롯에 할당
									for (int32 i = 0; i < QuickSlotComp->GetMaxSlots(); i++)
									{
										if (!QuickSlotComp->GetItemInSlot(i))
										{
											QuickSlotComp->AssignToSlot(i, AddedItem);
											AssignedSlot = i;
											UE_LOG(LogTemp, Warning, TEXT("TakePickup: 빈 퀵슬롯 %d번에 아이템 할당: %s"),
												AssignedSlot + 1, *AddedItem->GetName());
											break;
										}
									}

									// 모든 슬롯이 차있는 경우 첫 번째 슬롯에 강제 할당
									if (AssignedSlot == -1)
									{
										QuickSlotComp->AssignToSlot(0, AddedItem);
										AssignedSlot = 0;
									}
								}

								// 첫 번째 아이템을 획득한 경우에만 현재 퀵슬롯 인덱스 설정
								if (QuickSlotComp->GetTotalAssignedItems() == 1)
								{
									Taker->CurrentQuickSlotIndex = AssignedSlot;
								}
							}
						}
					}
				}

				// IsPickUp은 이미 PickUpAction_Server에서 설정되므로 여기서는 제거
				// Taker->IsPickUp = true; // 중복 설정 제거
			}

			// 전부 추가 성공 시에만 액터 파괴
			if (AddResult.OperationResult == EItemAddResult::TAR_AllItemAdded)
			{
				Destroy();
			}
		}
	}
}

void APickup::Server_TakePickup_Implementation(AActor* InteractingActor)
{
	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		// IsPickUp 상태 체크 - 이미 아이템 획득 중이면 추가 획득 차단
		if (PlayerCharacter->IsPickUp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server_TakePickup: 이미 아이템 획득 중 - 추가 획득 차단 (IsPickUp = true)"));
			return;
		}

		TakePickup(PlayerCharacter);
	}
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
	{
		if (ItemDataTable)
		{
			if (const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);
			}
		}
	}
}

#endif

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickup, ReplicatedItemData);
}