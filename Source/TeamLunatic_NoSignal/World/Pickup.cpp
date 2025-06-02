// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseWeapon.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseMagazine.h"


APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true); // 움직이는 아이템이면

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

		ItemReference = NewObject<UNS_InventoryBaseItem>(this, BaseClass);
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->WeaponType = ItemData->WeaponType;
		ItemReference->WeaponData = ItemData->WeaponData;
		ItemReference->TextData = ItemData->ItemTextData;
		ItemReference->NumericData = ItemData->ItemNumericData;
		ItemReference->AssetData = ItemData->ItemAssetData;
		ItemReference->ItemDataRowName = ItemData->ItemDataRowName;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		ItemReference->SetOwningActor(this);

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

	ItemReference->SetOwningActor(this);

	UpdateInteractableData();
}

void APickup::OnRep_ReplicatedItemData()
{
	if (!ItemReference)
	{
		ItemReference = NewObject<UNS_InventoryBaseItem>(this, UNS_InventoryBaseItem::StaticClass());
		ItemReference->SetOwningActor(this);
	}

	ItemReference->ItemDataRowName = ReplicatedItemData.ItemDataRowName;
	ItemReference->ItemsDataTable = ItemDataTable;
	ItemReference->TextData = ReplicatedItemData.ItemTextData;
	ItemReference->NumericData = ReplicatedItemData.ItemNumericData;
	ItemReference->AssetData = ReplicatedItemData.ItemAssetData;
	ItemReference->WeaponData = ReplicatedItemData.WeaponData;
	ItemReference->WeaponType = ReplicatedItemData.WeaponType;
	ItemReference->ItemType = ReplicatedItemData.ItemType;
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
		Server_TakePickup(InteractingActor); // 클라에서 서버로 요청
		return;
	}

	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter); // 서버에서 처리
	}
}

void APickup::TakePickup(ANS_PlayerCharacterBase* Taker)
{
	if (!HasAuthority()) return;

	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::TAR_NoItemAdded:
					break;
				case EItemAddResult::TAR_PartialAmountItemAdded:
					UpdateInteractableData();
					if (UInteractionComponent* InteractionComp = Taker->GetInteractionComponent())
					{
						InteractionComp->UpdateInteractionWidget();
					}
					break;
				case EItemAddResult::TAR_AllItemAdded:
					Destroy();
					break;
				}

				UE_LOG(LogTemp, Warning, TEXT("[TakePickup] AddResult: %d, Message: %s"),
					(int32)AddResult.OperationResult,
					*AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Inventory Component is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal Item reference was somehow null"));
		}
	}
}

void APickup::Server_TakePickup_Implementation(AActor* InteractingActor)
{
	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter);
	}
}

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

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickup, ReplicatedItemData);
}

