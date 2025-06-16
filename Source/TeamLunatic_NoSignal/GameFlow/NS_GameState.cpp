#include "NS_GameState.h"
#include "NS_PlayerState.h"
#include "Net/UnrealNetwork.h"

ANS_GameState::ANS_GameState()
{
    TrackingTarget = nullptr;
}

void ANS_GameState::SavePlayerData()
{
    for (APlayerState* PS : PlayerArray)
    {
        if (ANS_PlayerState* NSPlayerState = Cast<ANS_PlayerState>(PS))
        {
            NSPlayerState->SavePlayerData(); 
        }
    }
}

void ANS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_GameState, TrackingTarget);
}
