// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Zombies/NS_RunnerZombie.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Zombie/Enum/EZombieState.h"
#include "Zombie/Enum/EZombieType.h"

ANS_RunnerZombie::ANS_RunnerZombie()
{
	ZombieType = EZombieType::RUNNER;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Infected_Zombie_Bundle/Female_Infected/Mesh/SK_Woman_Zombie.SK_Woman_Zombie'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	MaxHealth = 70.f;
	BaseDamage = 10.f;
}

void ANS_RunnerZombie::OnStateChanged(EZombieState NewState)
{
	switch (NewState)
	{
	case EZombieState::IDLE:
	case EZombieState::DEAD:
	case EZombieState::DETECTING:
	case EZombieState::PUSHED:
		TargetSpeed = 0.f;
		break;
	case EZombieState::PATROLL:
		TargetSpeed = 20.f;
		break;
	case EZombieState::CHACING:
	case EZombieState::ATTACK:
		TargetSpeed = 350.f;
		break;
	default:
		break;
	}
}


