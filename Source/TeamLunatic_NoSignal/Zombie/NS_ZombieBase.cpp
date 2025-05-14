// Fill out your copyright notice in the Description page of Project Settings.
#include "Zombie/NS_ZombieBase.h"

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
}

void ANS_ZombieBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANS_ZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANS_ZombieBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

