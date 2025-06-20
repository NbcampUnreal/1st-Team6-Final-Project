#pragma once

#include "CoreMinimal.h"
#include "NS_GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class APawn;
class ANS_PlayerState;
class ANS_PlayerCharacterBase; // ANS_PlayerCharacterBase forward declaration

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

public:
    ANS_MultiPlayMode();

    // GameState에 등록된 추적 대상 위치 반환
    virtual FVector GetPlayerLocation_Implementation() const override;

    // ANS_GameModeBase의 OnPlayerCharacterDied_Implementation 함수를 오버라이드
    virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;


protected:
    virtual void BeginPlay() override;

    // 모든 플레이어를 PlayerStart 기준으로 스폰
    void SpawnAllPlayers();

    // 멀티플레이어 게임 오버 처리 로직
    void HandleGameOverMultiplayer(APlayerController* DeadPlayerController);

    // 게임 오버 상태를 추적하는 변수
    UPROPERTY()
    bool bIsGameOver = false;
};