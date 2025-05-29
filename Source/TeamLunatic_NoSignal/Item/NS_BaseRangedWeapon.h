#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "Item/NS_EAmmoType.h"
#include "NS_BaseRangedWeapon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class ANS_BaseMagazine;

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

	//나한테 안보임, 상대한테 보임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* RangedWeaponMeshComp;
	//나한테 보임, 상대한테 안보임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* ArmsMesh;

	//탄창 장착
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* AttachMagSound;

	//탄창 장착 해제
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* DetachMagSound;

	//총 발싸 실패
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USoundBase* UnFireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraSystem* NiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EAmmoType AmmoType;

	//현재 탄창
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ANS_BaseMagazine* CurrentMagazine;

	//탄창 착용
	UFUNCTION(BlueprintCallable)
	bool AttachMagazine(ANS_BaseMagazine* NewMagazine);

	//탄창 제거
	UFUNCTION(BlueprintCallable)
	ANS_BaseMagazine* DetachMagazine();

	//발사 체크
	UFUNCTION(BlueprintCallable)
	bool CanFire() const;

	//총 발사
	UFUNCTION(BlueprintCallable)
	void Fire();

	void PerformHitScan(int32 damage);
};
