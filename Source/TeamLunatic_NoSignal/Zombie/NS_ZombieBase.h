// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_ZombieBase.generated.h"

enum class Enum_ZombieType : uint8;
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
	// UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Mesh")
	// USkeletalMeshComponent* SkeletalMesh;
	// UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Mesh")
	// UAnimInstance* AnimInstance;

	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Stat")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	float CurrentHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	float BaseDamage;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	Enum_ZombieState CurrentState;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	Enum_ZombieType ZombieType;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION()
	virtual void Die();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void SetState(Enum_ZombieState NewState);
	UFUNCTION()
	virtual void OnStateChanged(Enum_ZombieState State);
	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(NetMulticast, reliable)
	void Die_Multicast();
	UFUNCTION(NetMulticast, reliable)
	void Attack_Multicast();
};
