// Fill out your copyright notice in the Description page of Project Settings.
#include "Zombie/NS_ZombieBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANS_ZombieBase::ANS_ZombieBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ZombieMeshAsset(TEXT("/Game/YI_ModularZombies/Meshes/ZombieF02/Zombie/SK_Zombie_F02_01"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ManequineAsset(TEXT("/Game/YI_ModularZombies/Demo/Characters/Mannequins/Meshes/SKM_Manny"));
	
	if (ManequineAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(ManequineAsset.Object);
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	}
	if (ZombieMeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(ZombieMeshAsset.Object);
	}
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	SphereComp = CreateDefaultSubobject<USphereComponent>("AttackRagne");

	MaxHealth = 100;
	CurrentHealth = MaxHealth;
}

void ANS_ZombieBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANS_ZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ANS_ZombieBase::Die()
{
	
	DetachFromControllerPendingDestroy();
	
	GetCharacterMovement()->DisableMovement();

	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	OnDeath_Implementation();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
}

void ANS_ZombieBase::OnDeath_Implementation()
{
	// 사망시 멀티캐스트로 처리할 것들.
}

