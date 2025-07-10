// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "Net/UnrealNetwork.h"
#include "Item/NS_ItemDataStruct.h"
#include "Pickup.generated.h"

class UDataTable;
class UNS_InventoryBaseItem;
class ANS_PlayerCharacterBase;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public INS_InteractionInterface
{
	GENERATED_BODY()

public:
	APickup();

	void InitializePickup(const TSubclassOf<UNS_InventoryBaseItem> BaseClass, const int32 InQuantity);
	void InitializeDrop(UNS_InventoryBaseItem* ItemToDrop, const int32 InQuantity);

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedItemData)
	FNS_ItemDataStruct ReplicatedItemData;

	// OnRep 함수 선언
	UFUNCTION()
	void OnRep_ReplicatedItemData();

	FORCEINLINE UNS_InventoryBaseItem* GetItemData() { return ItemReference; };
	FORCEINLINE UNS_InventoryBaseItem* GetItem() { return ItemReference; };
	FORCEINLINE int32 GetQuantity() const { return ItemQuantity; };

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

	void TakePickup(ANS_PlayerCharacterBase* Taker);
	UFUNCTION(Server, Reliable)
	void Server_TakePickup(AActor* InteractingActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	UDataTable* ItemDataTable;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	FName DesiredItemID;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | ItemReference")
	UNS_InventoryBaseItem* ItemReference;

		UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	int32 ItemQuantity;

	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	void UpdateInteractableData();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};