// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_BaseMeleeWeapon.generated.h"


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseMeleeWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()
	
protected:
	ANS_BaseMeleeWeapon();

	virtual void BeginPlay() override;

public:
	//나한테 안보임, 상대한테 보임
	//ItemStaticMesh

	//나한테 보임, 상대한테 안보임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ArmsMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Effects")
	UParticleSystem* ImpactParticles;
};
