// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NS_ZombieBase.generated.h"

class USphereComponent;
class UBoxComponent;
class ADecalActor;
class UNiagaraComponent;
class UNiagaraSystem;
class UPhysicalAnimationComponent;
class ANS_AIController;
class UMaterialInstanceDynamic;
enum class EZombieAttackType : uint8;
enum class EZombieType : uint8;
enum class EZombieState : uint8;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_ZombieBase();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Stat")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "Stat")
	float CurrentHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	float BaseDamage;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated ,Category = "Stat")
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

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentState, Category = "State")
	EZombieState CurrentState;

	UFUNCTION()
	void OnRep_CurrentState();
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "State")
	EZombieType ZombieType;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Replicated, Category = "State")
	EZombieAttackType CurrentAttackType;

	
	// Invoker 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UNavigationInvokerComponent* NavigationInvoker;
	
	// 이 좀비가 활성화 되었는지 확인 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bIsActive, Category = "Activation")
	bool bIsActive; // 기본적으로 비활성화 상태로 시작

	UFUNCTION()
	void OnRep_bIsActive();

	// 메쉬 가시성 강제 업데이트 (멀티플레이 문제 해결용)
	// UFUNCTION(NetMulticast, Reliable)
	// void ForceUpdateMeshVisibility_Multicast(bool bVisible);
	
	//피격관련
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(NetMulticast, reliable)
	void Die_Multicast();
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "State")
	UAnimMontage* KnockBackMontage;
	UFUNCTION(NetMulticast, reliable)
	void Multicast_PlayMontage(UAnimMontage* MontageToPlay);
	FTimerHandle HitTimer;
	void ResetHit();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	USoundCue* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit/Effect")
	TSubclassOf<AActor> BloodDecal;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Hit/Effect")
	TSubclassOf<AActor> BloodEffect;
	
	//Replicate
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Physics관련
	void InitializePhysics();
	UFUNCTION(NetMulticast, Unreliable)
	void ApplyPhysics(FName Bone, FVector Impulse);
	void ResetPhysics(FName Bone);
	UPROPERTY()
	TMap<FName, FTimerHandle> HitBoneTimers;
	UPROPERTY()
	TSet<FName> SafeBones;
	
	//상태 패턴
	UFUNCTION()
	void SetState(EZombieState NewState);
	UFUNCTION()
	virtual void OnStateChanged(EZombieState State);
	UFUNCTION()
	void SetAttackType(EZombieAttackType NewAttackType);

	UFUNCTION()
	void SetActive(bool NewIsActive);
	UFUNCTION(Server, Reliable)
	void Server_SetActive(bool NewIsActive);
	
	UFUNCTION(Server, Reliable)
	void Server_SetState(EZombieState NewState);
	UFUNCTION(Server, Reliable)
	void Server_SetAttackType(EZombieAttackType NewAttackType);

	//상태 별 전환 함수.
	void OnIdleState();
	void OnPatrolState();
	void OnDetectState();
	virtual void OnChaseState();
	virtual void OnAttackState();
	void OnDeadState();
	virtual void OnFrozenState();

	//이펙트
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnBloodEffect(FName Bone,FVector Location, FRotator Rotation);
 
	//공격 컴포넌트
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	USphereComponent* R_SphereComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attack")
	USphereComponent* L_SphereComp;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GotHit")
	TArray<UAnimMontage*> HitMontages;

	//사운드 관련
	UFUNCTION(Server, Reliable)
	void Server_PlaySound(USoundCue* Sound);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlaySound(USoundCue* Sound);
	UFUNCTION()
	void ScheduleSound(USoundCue* SoundCue);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* IdleSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* ChaseSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* DeathSound;
	
	FTimerHandle AmbientSoundTimer;

	//캐시 변수들
	UPROPERTY()
	TArray<UActorComponent*> Components;
	void CacheComponents();
	
	//Get함수
	FORCEINLINE const EZombieAttackType GetZombieAttackType() {return CurrentAttackType;}
	FORCEINLINE const EZombieState GetState() const {return CurrentState;};
	FORCEINLINE const EZombieType GetType() const {return ZombieType;}
	FORCEINLINE USphereComponent* GetR_SphereComponent() const {return R_SphereComp;}
	FORCEINLINE USphereComponent* GetL_SphereComponent() const {return L_SphereComp;}
};