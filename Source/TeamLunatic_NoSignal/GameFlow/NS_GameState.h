#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NS_GameState.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_GameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ANS_GameState();

    // 현재 밤 상태 (서버에서 계산, 클라이언트에 복제)
    UPROPERTY(ReplicatedUsing = OnRep_IsNight, BlueprintReadOnly, Category = "Time")
    bool bIsNight;

    // 현재 게임 시간 (0.0 ~ 24.0 기준)
    UPROPERTY(BlueprintReadOnly, Category = "Time")
    float TimeOfDay;

    // Tick으로 시간 추적
    virtual void Tick(float DeltaSeconds) override;
    virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
    UFUNCTION()
    void OnRep_IsNight();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateTimeFromDaySequence();
};
