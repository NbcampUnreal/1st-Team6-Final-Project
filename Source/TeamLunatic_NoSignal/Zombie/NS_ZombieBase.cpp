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
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ANS_ZombieBase::ANS_ZombieBase() : MaxHealth(100.f), CurrentHealth(MaxHealth), CurrentState(EZombieState::IDLE),
									BaseDamage(20.f),PatrolSpeed(20.f), ChaseSpeed(100.f),AccelerationSpeed(200.f), ZombieType(EZombieType::BASIC)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
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
	if (CurrentHealth<=0) return 0.f;
	float ActualDamage = DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	if (CurrentHealth <= 0)
	{
	
		Die_Multicast_Implementation();
	}
	return ActualDamage;
}

void ANS_ZombieBase::Die_Multicast_Implementation()
{
	DetachFromControllerPendingDestroy();
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
}


void ANS_ZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_ZombieBase, CurrentHealth);
	DOREPLIFETIME(ANS_ZombieBase, CurrentAttackType);
	DOREPLIFETIME(ANS_ZombieBase, CurrentState);
}

void ANS_ZombieBase::SetState(EZombieState NewState)
{
	if (!HasAuthority())
	{
		CurrentState = NewState;
		OnStateChanged(NewState);
	}
	else
	{
		Server_SetState_Implementation(NewState);
	}
}

void ANS_ZombieBase::OnStateChanged(EZombieState State)
{
	switch (State)
	{
	case EZombieState::IDLE:
	case EZombieState::DEAD:
	case EZombieState::PUSHED:
	case EZombieState::DETECTING:
		TargetSpeed = 0.f;
		break;
	case EZombieState::PATROLL:
		TargetSpeed = 40.f;
		break;
	case EZombieState::CHACING:
	case EZombieState::ATTACK:
		TargetSpeed = 200.f;
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
		Sever_SetAttackType_Implementation(NewAttackType);
	}
}

void ANS_ZombieBase::Server_SetState_Implementation(EZombieState NewState)
{
	CurrentState = NewState;
	OnStateChanged(NewState);
}

void ANS_ZombieBase::OnRep_State()
{
	OnStateChanged(CurrentState);
}

void ANS_ZombieBase::OnRep_AttackType()
{
}

void ANS_ZombieBase::Sever_SetAttackType_Implementation(EZombieAttackType NewAttackType)
{
	SetAttackType(NewAttackType);
}



