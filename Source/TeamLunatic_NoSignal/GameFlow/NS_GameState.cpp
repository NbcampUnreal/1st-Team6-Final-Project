#include "NS_GameState.h"
#include "NS_PlayerState.h"
#include "Net/UnrealNetwork.h"

ANS_GameState::ANS_GameState()
{
    TrackingTarget = nullptr;
}

void ANS_GameState::SavePlayerData()
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANS_PlayerState* PlayerState = Cast<ANS_PlayerState>(PlayerState))
		{
			PlayerState->SavePlayerData(); // 플레이어 상태에서 데이터 로드
		}
	}
}

void ANS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_GameState, TrackingTarget);
}
