#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ItemDataStruct.h"
#include "Engine/DataTable.h"
#include "NS_BaseWeapon.generated.h"

class ANS_PlayerCharacterBase;
class UInventoryComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ANS_BaseWeapon();

	UPROPERTY()
	UInventoryComponent* OwingInventory;

	virtual void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipcheck")
	bool isequip;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FName WeaponDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	int32 Weapondamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	float Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	float Current_Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* EquipSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* UnEquipSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* AttackSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category = "WeaponData")
	FItemAssetData ItemAssetData;

	UFUNCTION(Category = "WeaponData")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * ItemNumericData.Weight; };

	UFUNCTION(Category = "WeaponData")
	FORCEINLINE float GetItemSingleWeight() const { return ItemNumericData.Weight; };

	UFUNCTION(BlueprintCallable, Category = "WeaponData")
	void SetQuantity(int32 NewQuantity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	//UStaticMeshComponent* 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == ItemNumericData.MaxStack; };

	bool bisCopy;
	bool bisPickup;

	void ResetItemFlags();

	ANS_BaseWeapon* CreateItemCopy() const;
protected:
	bool operator==(const FName& OtherID) const
	{
		return this->WeaponDataRowName == OtherID;
	}
};
