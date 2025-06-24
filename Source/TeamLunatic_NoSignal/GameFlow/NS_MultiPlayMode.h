#pragma once

#include "CoreMinimal.h"
#include "NS_GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class ANS_PlayerCharacterBase;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

public:
    ANS_MultiPlayMode();

    virtual FVector GetPlayerLocation_Implementation() const override;
    virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;
    void SpawnAllPlayers();
    
    // 좀비 스폰 체크 함수 오버라이드
    virtual void CheckAndSpawnZombies() override;
    
    // 플레이어로부터 너무 멀리 있는 좀비 제거 함수 오버라이드
    virtual void CleanupDistantZombies() override;
    
    // 게임 오버 상태 플래그
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver = false;
    
protected:
    virtual void BeginPlay() override;
    
    // 랜덤한 살아있는 플레이어의 위치를 반환하는 함수
    FVector GetRandomPlayerLocation() const;
};
