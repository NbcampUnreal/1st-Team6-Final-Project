// Fill out your copyright notice in the Description page of Project Settings.
#include "NS_ZombieBase.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Enum/EZombieState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enum/EZombieType.h"
#include "Kismet/GameplayStatics.h"
#include "Enum/EZombieAttackType.h"
#include "AIController/NS_AIController.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

ANS_ZombieBase::ANS_ZombieBase() : MaxHealth(100.f), CurrentHealth(MaxHealth), CurrentState(EZombieState::IDLE),
                                   BaseDamage(20.f),PatrolSpeed(20.f), ChaseSpeed(100.f),AccelerationSpeed(200.f),
                                   ZombieType(EZombieType::BASIC), bIsDead(false), bIsGotHit(false), UnSafeBones({"pelvis","tight_l","calf_l","foot_l","tight_r","calf_r","foot_r", "root"})
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	PhysicsComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(FName("PhysicsComponent"));
	
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 72.0f, 0.0f);

	
	GetMesh()->SetRelativeLocation(FVector(0.f,0.f,-90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

	
	SphereComp = CreateDefaultSubobject<USphereComponent>("AttackRagne");
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComp->SetupAttachment(GetMesh());
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&ANS_ZombieBase::OnOverlapSphere);
	SphereComp->SetGenerateOverlapEvents(true);
}

void ANS_ZombieBase::BeginPlay()
{
	Super::BeginPlay();
	SetState(CurrentState);
	GetMesh()->GetAnimInstance()->RootMotionMode = ERootMotionMode::RootMotionFromEverything;
	InitializePhysics();
}

void ANS_ZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (!FMath::IsNearlyEqual(CurrentSpeed, TargetSpeed, 1.f)) // 1.f 오차 범위
	{
		float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, 2.f);
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

void ANS_ZombieBase::OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(Player, BaseDamage,GetController(), this, nullptr);
	}
}

float ANS_ZombieBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth<=0 || !HasAuthority()) return 0.f;
	float ActualDamage = DamageAmount;
	
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		const FPointDamageEvent* Point = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FName Bone = Point->HitInfo.BoneName;
		FVector HitDirection = Point->ShotDirection;
		if (Bone == "head")
		{
			ActualDamage*= 2.f;
		}
		ApplyPhysics(Bone, HitDirection * 3000.f);
	}
	
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	
	if (CurrentHealth <= 0)
	{
		SetState(EZombieState::DEAD);
	}

	return ActualDamage;
}

void ANS_ZombieBase::ApplyPhysics(FName Bone, FVector Impulse)
{
	if (UnSafeBones.Contains(Bone)) return;
	USkeletalMeshComponent* MeshComp = GetMesh();
	// FBodyInstance* BodyInstance = MeshComp->GetBodyInstance(Bone);
	// if (BodyInstance)
	// {
	// 	BodyInstance->SetInstanceSimulatePhysics(true);
	// 	BodyInstance->PhysicsBlendWeight = 1.f;
	// 	BodyInstance->AddImpulse(Impulse,true);
	// }
	GetMesh()->SetAllBodiesBelowSimulatePhysics(Bone,true,true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(Bone, 1.f, false);
	
	GetMesh()->AddImpulse(Impulse, Bone, false);

	// if (HitTimers.Contains(Bone))
	// {
	// 	GetWorldTimerManager().ClearTimer(HitTimers[Bone]);
	// }

	// FTimerHandle& NewTimer = HitTimers.FindOrAdd(Bone);
	// GetWorldTimerManager().SetTimer(NewTimer, [this, Bone]()
	// {
	// 	ResetPhysics(Bone);
	// }, 1.f, false);
}

void ANS_ZombieBase::ResetPhysics(FName Bone)
{
	if (CurrentState == EZombieState::DEAD) return;
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(Bone, 0.f, false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(Bone, false, true);
}

void ANS_ZombieBase::OnIdleState()
{
	TargetSpeed = 0.f;
}

void ANS_ZombieBase::OnPatrolState()
{
	TargetSpeed = 70.f;
}

void ANS_ZombieBase::OnDetectState()
{

}

void ANS_ZombieBase::OnChaseState()
{
	TargetSpeed = 600.f;
}

void ANS_ZombieBase::OnAttackState()
{
	TargetSpeed = 600.f;
}

void ANS_ZombieBase::OnDeadState()
{
	if (bIsDead) return;
	if (HasAuthority())
	{
		bIsDead = true;
		Die_Multicast();
	}
}

void ANS_ZombieBase::OnFrozenState()
{
}

void ANS_ZombieBase::Server_PlaySound_Implementation(USoundCue* Sound)
{
	Multicast_PlaySound(Sound);
}

void ANS_ZombieBase::Multicast_PlaySound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetMesh()->GetSocketLocation("headsocket"));
	}
}

void ANS_ZombieBase::Die_Multicast_Implementation()
{
	DetachFromControllerPendingDestroy();
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()-> SetAllBodiesPhysicsBlendWeight(1.f);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(30.f);
}

void ANS_ZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_ZombieBase, CurrentHealth);
	DOREPLIFETIME(ANS_ZombieBase, CurrentAttackType);
	DOREPLIFETIME(ANS_ZombieBase, CurrentState);
}

void ANS_ZombieBase::InitializePhysics()
{
	if (PhysicsComponent)
	{
		PhysicsComponent->SetSkeletalMeshComponent(GetMesh());

		FPhysicalAnimationData Config;
		Config.bIsLocalSimulation = true;
		Config.OrientationStrength = 50.f;
		Config.PositionStrength = 100.f;
		Config.VelocityStrength = 75.f;

		PhysicsComponent->ApplyPhysicalAnimationSettingsBelow(FName("pelvis"), Config,true) ;
	}
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetAllBodiesPhysicsBlendWeight(0.f);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}


void ANS_ZombieBase::SetState(EZombieState NewState)
{
	if (HasAuthority())
	{
		CurrentState = NewState;
		OnStateChanged(CurrentState);
	}
	else
	{
		Server_SetState(CurrentState);
	}
}

void ANS_ZombieBase::OnStateChanged(EZombieState State)
{
	switch (State)
	{
	case EZombieState::IDLE:
		OnIdleState();
		break;
	case EZombieState::DEAD:
		OnDeadState();
		break;
	case EZombieState::PUSHED:
	case EZombieState::DETECTING:
		OnDetectState();
		break;
	case EZombieState::PATROLL:
		OnPatrolState();
		break;
	case EZombieState::CHACING:
		OnChaseState();
		break;
	case EZombieState::ATTACK:
		OnAttackState();
		break;
	case EZombieState::FROZEN:
		OnFrozenState();
		break;
	default:
		break;
	}
}

void ANS_ZombieBase::SetAttackType(EZombieAttackType NewAttackType)
{
	if (HasAuthority())
	{
		CurrentAttackType = NewAttackType;
	} 
	else
	{
		Server_SetAttackType(NewAttackType);
	}
}


// void ANS_ZombieBase::MontagePlay(ANS_AIController* NSController, UAnimMontage* MontageToPlay)
// {
// 	if (!NSController || !MontageToPlay) return;
// 	NSController->PauseBT();
// 	
// 	float Duration = MontageToPlay->GetPlayLength();
// 	GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
// 	
// 	FTimerDelegate MontageDelegate;
// 	MontageDelegate.BindUFunction(this, FName("OnEndMontagePlay"), NSController);
// 	GetWorldTimerManager().SetTimer(MontageTimerHandle,MontageDelegate, Duration,false);
// }
//
// void ANS_ZombieBase::OnEndMontagePlay(ANS_AIController* NSController)
// {
// 	if (NSController)
// 	{
// 		NSController->ResumeBT();
// 	}
// }

void ANS_ZombieBase::Server_SetState_Implementation(EZombieState NewState)
{
	CurrentState = NewState;
	OnStateChanged(CurrentState);
}

void ANS_ZombieBase::Server_SetAttackType_Implementation(EZombieAttackType NewAttackType)
{
	CurrentAttackType = NewAttackType;
	SetAttackType(CurrentAttackType);
}