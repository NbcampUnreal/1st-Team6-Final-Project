#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "Item/NS_EAmmoType.h"
#include "NS_BaseRangedWeapon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseRangedWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()

protected:
	ANS_BaseRangedWeapon();

	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMesh* RangedWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* RangedWeaponMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* ArmsMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* UnFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraSystem* NiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo")
	EAmmoType AmmoType;

	FORCEINLINE int32 GetMaxAmmo() const { return MaxAmmo; }
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }

	void Reload(int32 AmmoToAdd);
};
