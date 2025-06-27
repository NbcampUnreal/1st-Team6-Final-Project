#include "NS_GameState.h"
#include "NS_PlayerState.h"
#include "Character/NS_PlayerController.h"
#include "GameFramework/PlayerController.h" 
#include "Kismet/GameplayStatics.h"
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

void ANS_GameState::Multicast_UpdateAllTipTexts_Implementation(const FText& Message)
{
    if (GetWorld())
    {
        ANS_PlayerController* PC = Cast<ANS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (PC)
        {
            PC->UpdateTipHUD(Message);
        }
    }
}