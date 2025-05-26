//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "Interaction/InteractionInterface.h"
//#include "Pickup.generated.h"
//
//class UDataTable;
//class ANS_BaseItem;
//class ANS_PlayerCharacterBase;
//
//UCLASS()
//class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public IInteractionInterface
//{
//	GENERATED_BODY()
//
//public:
//	APickup();
//
//	void InitializePickup(const TSubclassOf<ANS_BaseItem> BaseClass, const int32 InQuantity);
//	void InitializeDrop(ANS_BaseItem* ItemToDrop, const int32 InQuantity);
//
//	FORCEINLINE ANS_BaseItem* GetItemData() { return ItemReference; };
//
//	virtual void BeginFocus() override;
//	virtual void EndFocus() override;
//protected:
//	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
//	UStaticMeshComponent* PickupMesh;
//
//	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
//	UDataTable* ItemDataTable;
//
//	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
//	FName DesiredItemID;
//
//	UPROPERTY(VisibleAnywhere, Category = "Pickup | ItemReference")
//	ANS_BaseItem* ItemReference;
//
//	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
//	int32 ItemQuantity;
//
//	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
//	FInteractableData InstanceInteractableData;
//
//	virtual void BeginPlay() override;
//	virtual void Interact(AActor* InteractingActor) override;
//	void UpdateInteractableData();
//	void TakePickup(ANS_PlayerCharacterBase* Taker);
//
//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
//};