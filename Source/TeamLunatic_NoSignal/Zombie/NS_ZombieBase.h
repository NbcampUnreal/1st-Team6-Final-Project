// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_ZombieBase.generated.h"


enum class Enum_ZombieState : uint8;
class USphereComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_ZombieBase();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USphereComponent* SphereComp;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Stat")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	Enum_ZombieState CurrentState;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void  GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	virtual void Die();

	void SetState(Enum_ZombieState NewState);
	
	UFUNCTION(NetMulticast, Reliable)
	void Die_Multicast();
};
