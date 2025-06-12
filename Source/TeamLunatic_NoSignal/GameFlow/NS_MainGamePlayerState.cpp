#include "GameFlow/NS_MainGamePlayerState.h"
#include "Net/UnrealNetwork.h"

ANS_MainGamePlayerState::ANS_MainGamePlayerState()
{
    // 기본 생성자
}

void ANS_MainGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_MainGamePlayerState, SelectedPawnClass);
    DOREPLIFETIME(ANS_MainGamePlayerState, PlayerIndex);
}
