#include "GameFlow/NS_MainGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/NS_UIManager.h"
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
                    if (UNS_UIManager* UIManager = GI->GetUIManager())
                    {
                        UWorld* World = PC->GetWorld();

                        if (World)
                        {
                            UE_LOG(LogTemp, Warning, TEXT(" World 유효. NetMode: %d"), static_cast<int32>(World->GetNetMode()));
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("World is NULL"));
                        }

                        UIManager->ShowGameOverWidget(World);
                    }
                }
            }
        }
    }
}
