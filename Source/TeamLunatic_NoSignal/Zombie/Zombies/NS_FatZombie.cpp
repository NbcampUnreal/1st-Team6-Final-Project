// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Zombies/NS_FatZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zombie/Enum/Enum_ZombieState.h"
#include "Zombie/Enum/Enum_ZombieType.h"

ANS_FatZombie::ANS_FatZombie()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Infected_Zombie_Bundle/Infected_Zombie/Mesh/SK_Infected_zombie"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	MaxHealth = 200.f;
	BaseDamage = 40.f;
	ZombieType = Enum_ZombieType::FAT;
}

void ANS_FatZombie::OnStateChanged(Enum_ZombieState NewState)
{
	switch (NewState)
	{
	case Enum_ZombieState::DEAD:
	case Enum_ZombieState::IDLE:
	case Enum_ZombieState::DETECTING:
	case Enum_ZombieState::PUSHED:
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		break;
	case Enum_ZombieState::PATROLL:
		GetCharacterMovement()->MaxWalkSpeed = 10.f;
		break;
	case Enum_ZombieState::CHACING:
	case Enum_ZombieState::ATTACK:
		GetCharacterMovement()->MaxWalkSpeed = 100.f;
		break;
	default:
		break;
	}
}
