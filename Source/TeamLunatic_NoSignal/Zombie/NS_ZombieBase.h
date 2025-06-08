// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_ZombieBase.generated.h"

enum class EZombieAttackType : uint8;
enum class EZombieType : uint8;
enum class EZombieState : uint8;
class USphereComponent;


UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_ZombieBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Stat")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	float CurrentHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	float BaseDamage;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float PatrolSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float ChaseSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float AccelerationSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float TargetSpeed;
	
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	EZombieState CurrentState;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	EZombieType ZombieType;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	EZombieAttackType CurrentAttackType;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(NetMulticast, reliable)
	void Die_Multicast();
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void SetState(EZombieState NewState);
	UFUNCTION()
	virtual void OnStateChanged(EZombieState State);
	UFUNCTION()
	void SetAttackType(EZombieAttackType NewAttackType);
	
	UFUNCTION(Server, Reliable)
	void Server_SetState(EZombieState NewState);
	UFUNCTION(Server, Reliable)
	void Server_SetAttackType(EZombieAttackType NewAttackType);
	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	USphereComponent* SphereComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* BasicAttack;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* ChargeAttack;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* JumpAttack;
	

	FORCEINLINE const EZombieAttackType GetZombieAttackType() {return CurrentAttackType;}
	FORCEINLINE const EZombieState GetState() const {return CurrentState;};
	FORCEINLINE const EZombieType GetType() const {return ZombieType;}
	FORCEINLINE USphereComponent* GetSphereComp() const {return SphereComp;}
};



