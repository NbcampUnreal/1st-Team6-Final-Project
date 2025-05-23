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
	
public:
	ANS_BaseWeapon();

protected:
	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 WeaponDamage;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	FName SocketName;

public:
};
