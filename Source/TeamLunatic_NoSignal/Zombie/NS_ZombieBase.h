// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_ZombieBase.generated.h"

class UPhysicalAnimationComponent;
class ANS_AIController;
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
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	bool bIsDead;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float PatrolSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float ChaseSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float AccelerationSpeed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Speed")
	float TargetSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Hit Reaction")
	UPhysicalAnimationComponent* PhysicsComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "State")
	EZombieState CurrentState;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "State")
	EZombieType ZombieType;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "State")
	EZombieAttackType CurrentAttackType;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "State")
	bool bIsGotHit;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(NetMulticast, reliable)
	void Die_Multicast();
public:
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Physics관련
	void InitializePhysics();
	void ApplyPhysics(FName Bone, FVector Impulse);
	void ResetPhysics(FName Bone);
	UPROPERTY()
	TMap<FName, FTimerHandle> HitTimers;
	UPROPERTY()
	TSet<FName> UnSafeBones;
	
	//상태 패턴
	UFUNCTION()
	void SetState(EZombieState NewState);
	UFUNCTION()
	virtual void OnStateChanged(EZombieState State);
	UFUNCTION()
	void SetAttackType(EZombieAttackType NewAttackType);

	//상태 별 전환 함수.
	void OnIdleState();
	void OnPatrolState();
	void OnDetectState();
	virtual void OnChaseState();
	virtual void OnAttackState();
	void OnDeadState();
	virtual void OnFrozenState();
	
	//사운드 관련
	UFUNCTION(Server, Reliable)
	void Server_PlaySound(USoundCue* Sound);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlaySound(USoundCue* Sound);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sound")
	USphereComponent* SphereComp;
	
	//공격 관련
	UFUNCTION(Server, Reliable)
	void Server_SetState(EZombieState NewState);
	UFUNCTION(Server, Reliable)
	void Server_SetAttackType(EZombieAttackType NewAttackType);
	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GotHit")
	TArray<UAnimMontage*> HitMontages;
	
	//Get함수
	FORCEINLINE const EZombieAttackType GetZombieAttackType() {return CurrentAttackType;}
	FORCEINLINE const EZombieState GetState() const {return CurrentState;};
	FORCEINLINE const EZombieType GetType() const {return ZombieType;}
	FORCEINLINE USphereComponent* GetSphereComp() const {return SphereComp;}
};



