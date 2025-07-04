#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NS_GameState.generated.h"

class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_GameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ANS_GameState();

    // 현재 좀비가 감시할 플레이어 (멀티 전체 공유) 
    UPROPERTY(BlueprintReadOnly, Replicated)
    APawn* TrackingTarget;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_UpdateAllTipTexts(const FText& Message);

};
