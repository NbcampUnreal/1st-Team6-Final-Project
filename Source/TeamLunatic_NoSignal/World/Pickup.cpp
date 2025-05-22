// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "Item/NS_BaseItem.h"
#include "Inventory/InventoryComponent.h"
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

	InitializePickup(ANS_BaseItem:: StaticClass(), ItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<ANS_BaseItem> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FNS_ItemDataStruct* ItemData = ItemDataTable->FindRow<FNS_ItemDataStruct>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<ANS_BaseItem>(this, BaseClass);

		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->TextData = ItemData->ItemTextData;
		ItemReference->NumericData = ItemData->ItemNumericData;
		ItemReference->AssetData = ItemData->ItemAssetData;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(ANS_BaseItem* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.StaticMesh);

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

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
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

