#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_BaseWeapon.generated.h"

class USphereComponent;
class UBoxComponent;
class UCapsuleComponent;
class UNS_InventoryBaseItem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseWeapon : public ANS_BaseItem
{
	GENERATED_BODY()
	
protected:
	ANS_BaseWeapon();

	virtual void BeginPlay() override;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	//EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	float Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	float Current_Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* AttackSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* EquipSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* UnEquipSound = nullptr;

	EWeaponType GetWeaponType() const { return WeaponType; }

	FName GetAttachSocketName() const { return AttachSocketName; }

	int32 GetDamage() const { return Damage; }

	UPROPERTY()
	TObjectPtr<UNS_InventoryBaseItem> OwningInventoryItem;
};
