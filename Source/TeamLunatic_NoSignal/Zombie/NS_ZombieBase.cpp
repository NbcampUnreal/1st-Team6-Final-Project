// Fill out your copyright notice in the Description page of Project Settings.
#include "NS_ZombieBase.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Enum/Enum_ZombieState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enum/Enum_ZombieType.h"
#include "AIController/NS_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ANS_ZombieBase::ANS_ZombieBase() : MaxHealth(100.f), CurrentHealth(MaxHealth), CurrentState(Enum_ZombieState::IDLE),ZombieType(Enum_ZombieType::BASIC), BaseDamage(20.f)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetMesh()->SetRelativeLocation(FVector(0.f,0.f,-90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f,-90.f,0.f));
	SphereComp = CreateDefaultSubobject<USphereComponent>("AttackRagne");
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComp->SetupAttachment(GetMesh());
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&ANS_ZombieBase::OnOverlapSphere);
}

void ANS_ZombieBase::BeginPlay()
{
	Super::BeginPlay();
	SetState(CurrentState);
}

void ANS_ZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (!HasAuthority()||CurrentHealth<=0) return 0.f;
	float ActualDamage = DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	if (CurrentHealth <= 0)
	{
		Die();
	}
	return ActualDamage;
}

void ANS_ZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_ZombieBase, CurrentHealth);
	DOREPLIFETIME(ANS_ZombieBase, ZombieType);
	DOREPLIFETIME(ANS_ZombieBase, CurrentState);
	
}

void ANS_ZombieBase::Die()
{
	if (!HasAuthority()) return;
	
	DetachFromControllerPendingDestroy();
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true, true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
	Die_Multicast_Implementation();
}



void ANS_ZombieBase::Die_Multicast_Implementation()
{
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
}

void ANS_ZombieBase::SetState(Enum_ZombieState NewState)
{
	CurrentState = NewState;
	OnStateChanged(CurrentState);
}

void ANS_ZombieBase::OnStateChanged(Enum_ZombieState State)
{
	switch (State)
	{
	case Enum_ZombieState::IDLE:
	case Enum_ZombieState::DEAD:
	case Enum_ZombieState::PUSHED:
	case Enum_ZombieState::DETECTING:
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		break;
	case Enum_ZombieState::PATROLL:
		GetCharacterMovement()->MaxWalkSpeed = 20.f;
		break;
	case Enum_ZombieState::CHACING:
	case Enum_ZombieState::ATTACK:
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
		break;
	default:
		break;
	}
}


//공격 시 피격효과 멀티캐스팅
void ANS_ZombieBase::Attack_Multicast_Implementation()
{
}
