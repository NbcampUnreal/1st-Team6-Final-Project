#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_BaseWeapon.generated.h"

class USphereComponent;
class UBoxComponent;
class UCapsuleComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|HitDetection")
	bool bUseBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|HitDetection")
	bool bUseSphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|HitDetection")
	bool bUseCapsuleComponent;

	EWeaponType GetWeaponType() const { return WeaponType; }

	FName GetAttachSocketName() const { return AttachSocketName; }

	int32 GetDamage() const { return Damage; }
};
