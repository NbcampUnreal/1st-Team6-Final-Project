#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_BaseThrowWeapon.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseThrowWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()

protected:
	ANS_BaseThrowWeapon();

	virtual void BeginPlay() override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ArmsMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Effects")
	UParticleSystem* ImpactParticles;
};
