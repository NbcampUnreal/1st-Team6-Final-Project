#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_BaseMeleeWeapon.generated.h"

class UStaticMeshComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseMeleeWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()
	
public:
	ANS_BaseMeleeWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* WeaponMesh;


};
