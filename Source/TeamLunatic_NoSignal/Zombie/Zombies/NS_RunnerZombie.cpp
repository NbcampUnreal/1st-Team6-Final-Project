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
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ANS_RunnerZombie::OnChaseState()
{
	TargetSpeed = 550.f;
}


