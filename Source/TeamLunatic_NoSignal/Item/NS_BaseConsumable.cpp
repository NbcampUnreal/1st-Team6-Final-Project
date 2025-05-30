#include "Item/NS_BaseConsumable.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

ANS_BaseConsumable::ANS_BaseConsumable()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
}

void ANS_BaseConsumable::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		Maxstack = ItemData->ItemNumericData.MaxStack;
	}

	Currentstack = 0;

	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Name = ItemName;
	InstanceInteractableData.Action = FText::FromString("to pick up");
}

void ANS_BaseConsumable::Consume()
{
	Currentstack--;

	PlayUseEffectSound();
}

void ANS_BaseConsumable::OnUseItem()
{
	if (bIsBeingUsed || Currentstack <= 0)
		return;

	bIsBeingUsed = true;

	Consume();
}

void ANS_BaseConsumable::AddItem(int32 Amount)
{
	if (Currentstack >= Maxstack)
	{
		return;
	}

	Currentstack += Amount;
}

void ANS_BaseConsumable::RemoveStack(int32 Amount)
{
	if (Currentstack <= 0)
	{
		return;
	}

	Currentstack -= Amount;
}

void ANS_BaseConsumable::PlayUseEffectSound()
{
	if (UseSound)
	{
		UGameplayStatics::PlaySound2D(this, UseSound);
	}

	if (UseEffect)
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				UseEffect,
				OwnerPawn->GetActorLocation()
			);
		}
	}
}
