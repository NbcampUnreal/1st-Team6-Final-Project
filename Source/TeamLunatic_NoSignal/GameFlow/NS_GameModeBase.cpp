#include "GameFlow/NS_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/NS_ZombieBase.h"
#include "Algo/RandomShuffle.h"
#include "Zombie/Zombies/NS_BasicZombie.h"
#include "Zombie/Zombies/NS_FatZombie.h"
#include "Zombie/Zombies/NS_RunnerZombie.h"
#include "Zombie/Zombies/NS_Chaser.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Zombie/ZombieActivateManager/NS_ZombieActivationManager.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "GameFramework/Character.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "GameFlow/NS_MainGamePlayerState.h"


ANS_GameModeBase::ANS_GameModeBase()
{
}

// 플레이어 위치 반환
FVector ANS_GameModeBase::GetPlayerLocation_Implementation() const
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void ANS_GameModeBase::OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter)
{
	// 기본 구현: 플레이어가 죽었을 때의 기본 처리
	if (!DeadCharacter)
	{
		return;
	}
	// 플레이어 상태 업데이트
	if (AController* Controller = DeadCharacter->GetController())
	{
		if (ANS_MainGamePlayerState* PS = Controller->GetPlayerState<ANS_MainGamePlayerState>())
		{
			PS->bIsAlive = false;
		}
	}
}

