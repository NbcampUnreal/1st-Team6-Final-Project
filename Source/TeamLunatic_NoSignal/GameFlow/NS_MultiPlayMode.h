#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class AActor;
class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANS_MultiPlayMode();

protected:
	virtual void BeginPlay() override;

	/** 모든 플레이어를 스폰 */
	void SpawnAllPlayers();

private:
	/** 설정 */
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<APawn> PlayerPawnClass;
};
