#include "GameFlow/NS_MainGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "NS_GameInstance.h"

ANS_MainGamePlayerState::ANS_MainGamePlayerState()
{
    // 기본 생성자
}



void ANS_MainGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_MainGamePlayerState, SelectedPawnClass);
	DOREPLIFETIME(ANS_MainGamePlayerState, bIsAlive);
}

void ANS_MainGamePlayerState::OnRep_IsAlive()
{
    if (!bIsAlive)
    {
        if (APlayerController* PC = GetPlayerController())
        {
            if (PC->IsLocalController())
            {

                if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(PC->GetGameInstance()))
                {
                    // 캐릭터가 사망한 뒤에 게임 오버 UI 창 띄우기
                }
            }
        }
    }
}
