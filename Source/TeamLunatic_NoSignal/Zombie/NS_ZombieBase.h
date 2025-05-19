// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_ZombieBase.generated.h"

class USphereComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANS_ZombieBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USphereComponent* SphereComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CurrentHealth;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Die();
};
