// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Item/NS_ItemDataStruct.h"

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

	//InitializePickup(UItemBase :: StaticClass(), ItemQuantity);
}

//void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
//{
	//if (ItemDataTable && !DesiredItemID.IsNone())
	//{
		//const FItemData* ItemData = ItemDataTable->FindRow<FItemData>()
	//}
//}

//void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
//{
//}

void APickup::BeginFocus()
{
}

void APickup::EndFocus()
{
}

void APickup::Interact()
{
}

void APickup::TakePickup(const ANS_PlayerCharacter* Taker)
{
}

