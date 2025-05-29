#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
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

	//나한테 안보임, 상대한테 보임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* RangedWeaponMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	UStaticMeshComponent* RangeWeaponStaticMeshComp;
	//나한테 보임, 상대한테 안보임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	USkeletalMeshComponent* ArmsMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	UStaticMeshComponent* ArmsMeshStaticMeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX")
	UNiagaraSystem* NiagaraEffect;
};
