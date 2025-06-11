// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Zombies/NS_FatZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zombie/Enum/EZombieState.h"
#include "Zombie/Enum/EZombieType.h"

ANS_FatZombie::ANS_FatZombie()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Infected_Zombie_Bundle/Infected_Zombie/Mesh/SK_Infected_zombie"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;
	BaseDamage = 40.f;
	ZombieType = EZombieType::FAT;
}

void ANS_FatZombie::OnChaseState()
{
	TargetSpeed = 140.f;
}

void ANS_FatZombie::OnAttackState()
{
	TargetSpeed = 140.f;
}

