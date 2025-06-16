#pragma once

#include "CoreMinimal.h"
#include "NS_GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class APawn;
class ANS_PlayerState;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

public:
    ANS_MultiPlayMode();

    // GameState에 등록된 추적 대상 위치 반환 
    virtual FVector GetPlayerLocation_Implementation() const override;
protected:
    virtual void BeginPlay() override;

    // 모든 플레이어를 PlayerStart 기준으로 스폰 
    void SpawnAllPlayers();

};
