#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ItemDataStruct.h"
#include "Engine/DataTable.h"
#include "Interaction/InteractionInterface.h"
#include "NS_BaseItem.generated.h"

class UInventoryComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ANS_BaseItem();

	UInventoryComponent* OwingInventory;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Confige")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Confige")
	FName ItemDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* GetItemSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	FInteractableData InstanceInteractableData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemAssetData AssetData;

	UPROPERTY(VisibleAnywhere, Category = "ItemData", meta = (UIMin=1, UIMax=100))
	int32 Quantity;

	bool bisCopy;
	bool bisPickup;

	void ResetItemFlags();

	ANS_BaseItem* CreateItemCopy();

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

	virtual void Tick(float DeltaTime) override;

	virtual void OnUseItem();

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	bool operator == (const FName& OtherID) const
	{
		return ItemDataRowName == OtherID;
	}
};
