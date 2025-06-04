// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Inventory/InventoryComponent.h"
#include "Engine/DataTable.h"
#include "Item/NS_ItemDataStruct.h"
#include "NS_InventoryBaseItem.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryBaseItem : public UObject
{
	GENERATED_BODY()
	
public:
	UNS_InventoryBaseItem();

	UInventoryComponent* OwingInventory;

	AActor* OwningActor = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetOwningActor(AActor* NewOwner);

	UFUNCTION(BlueprintCallable)
	AActor* GetOwningActor() const;

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	int32 GetQuantity() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Confige")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Confige", Replicated)
	FName ItemDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData", Replicated)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData", Replicated)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData", Replicated)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* GetItemSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FItemStates ItemStates;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FItemAssetData AssetData;

	UPROPERTY(EditAnywhere, Category = "ItemData", Replicated)
	FConsumAbleItemAssetData ConsumableItemAssetData;

	UPROPERTY(VisibleAnywhere, Category = "ItemData", meta = (UIMin = 1, UIMax = 100), Replicated)
	int32 Quantity;

	bool bisCopy;
	bool bisPickup;

	void ResetItemFlags();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UNS_InventoryBaseItem* CreateItemCopy();

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * NumericData.Weight; };

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; };

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.MaxStack; };

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);
public:
	EItemType GetItemType() const { return ItemType; }
	EWeaponType GetWeaponType() const { return WeaponType; }
	FText GetItemName() const { return ItemName; }
	float GetWeight() const { return Weight; }
	const FNS_ItemDataStruct* GetItemData() const;

	virtual void OnUseItem(class ANS_PlayerCharacterBase* Character);
	void EquipWeapon(const FNS_ItemDataStruct* ItemData);
	void EquipMagazine(const FNS_ItemDataStruct* ItemData);
	void UseAmmo(const FNS_ItemDataStruct* ItemData);
	bool IsSupportedForNetworking() const;
protected:
	bool operator == (const FName& OtherID) const
	{
		return ItemDataRowName == OtherID;
	}
};
