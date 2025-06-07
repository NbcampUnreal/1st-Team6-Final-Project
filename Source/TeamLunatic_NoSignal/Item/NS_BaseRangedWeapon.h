#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "Item/NS_EAmmoType.h"
#include "NS_BaseRangedWeapon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
// 캐릭터 전방선언 지우지마세요 
class ANS_PlayerCharacterBase;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseRangedWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()

protected:
	ANS_BaseRangedWeapon();

	virtual void BeginPlay() override;

public:
	// // 무기별 사격 애니메이션때문에 캐릭터 변수가 필요해서 지우면 안됩니다 사운드랑 나이아가라변수만 지워주세요
	// UPROPERTY(BlueprintReadWrite, Category = "Owner")
	// ANS_PlayerCharacterBase* OwnerCharacter;
	//
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
