// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseWeapon.h"
#include "Character/NS_PlayerCharacterBase.h"


APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(ANS_BaseWeapon:: StaticClass(), ItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<ANS_BaseWeapon> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<ANS_BaseWeapon>(this, BaseClass);

		ItemReference->WeaponType = ItemData->WeaponType;
		ItemReference->ItemTextData = ItemData->ItemTextData;
		ItemReference->ItemNumericData = ItemData->ItemNumericData;
		ItemReference->ItemAssetData = ItemData->ItemAssetData;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(ANS_BaseWeapon* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->ItemNumericData.Weight = ItemToDrop->GetItemSigleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->ItemAssetData.StaticMesh);

	UpdateInteractableData();
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->ItemTextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->ItemTextData.ItemName;
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

void APickup::Interact(AActor* InteractingActor)
{
	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(InteractingActor))
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(ANS_PlayerCharacterBase* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			//if(UInventoryComponent* PlayerInventory = Taker->GetInventory())

				//try to add Item to player Inventory
				//based on result of the add operation
				//adjust or destroy the pick up
		}
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

