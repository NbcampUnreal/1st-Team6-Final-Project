// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "Pickup.generated.h"

<<<<<<< HEAD
=======
class UDataTable;
class ANS_BaseWeapon;
class ANS_PlayerCharacterBase;

>>>>>>> parent of 189edb63a ([Fix]인벤토리 컴포넌트, 픽업 수정 #7)
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	APickup();

<<<<<<< HEAD
	UStaticMeshComponent* PickupMesh;
=======
	void InitializePickup(const TSubclassOf<ANS_BaseWeapon> BaseClass, const int32 InQuantity);
	void InitializeDrop(ANS_BaseWeapon* ItemToDrop, const int32 InQuantity);

	FORCEINLINE ANS_BaseWeapon* GetItemData() { return ItemReference; };

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	FName DesiredItemID;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup | ItemReference")
	ANS_BaseWeapon* ItemReference;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	int32 ItemQuantity;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;
	
	virtual void BeginPlay() override;
	virtual void Interact(AActor* InteractingActor) override;
	void UpdateInteractableData();
	void TakePickup(ANS_PlayerCharacterBase* Taker);
>>>>>>> parent of 189edb63a ([Fix]인벤토리 컴포넌트, 픽업 수정 #7)

	UDataTable* ItemDataTable;

	FName DesiredItemID;

	/*UItemBase* ItemReference;*/

	int32 ItemQuantity;

	FInteractableData InstanceInteractableData;

	/*void InitializePickup(const TSubclassOf<UItembase> BaseClass, const int32 InQuantity);*/

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
