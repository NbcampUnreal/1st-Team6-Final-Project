#include "NS_GameState.h"
#include "Net/UnrealNetwork.h"

ANS_GameState::ANS_GameState()
{
    TrackingTarget = nullptr;
}

void ANS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_GameState, TrackingTarget);
}
