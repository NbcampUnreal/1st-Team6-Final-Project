#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NS_SinglePlayMode.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_SinglePlayMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANS_SinglePlayMode();

protected:
	virtual void BeginPlay() override;

	// 플레이어 스폰
	void SpawnPlayer();

	// 좀비/아이템 스폰
	void SpawnZombies();
	void SpawnItems();

	// 게임 오버
	void HandleGameOver(bool bPlayerSurvived);

private:
	bool bIsGameOver = false;

	// 스폰 설정
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<APawn> PlayerPawnClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> ZombieClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 ZombieCount = 100;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 ItemCount = 500;
};
