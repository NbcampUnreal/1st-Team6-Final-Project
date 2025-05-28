// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Zombies/NS_BasicZombie.h"

ANS_BasicZombie::ANS_BasicZombie()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Infected_Zombie_Bundle/Ghoul_Zobmie/Mesh/SK_Zmobie"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
}
