#include "NS_PlayerState.h"
#include "Net/UnrealNetwork.h"

ANS_PlayerState::ANS_PlayerState()
{
	bIsReady = false;
	bReplicates = true;
}

void ANS_PlayerState::SetIsReady(bool bReady)
{
	if (HasAuthority())
	{
		bIsReady = bReady;
		OnRep_IsReady();
	}
}

void ANS_PlayerState::OnRep_IsReady()
{
	UE_LOG(LogTemp, Log, TEXT("Player %s is now %s"),
		*GetPlayerName(),
		bIsReady ? TEXT("READY") : TEXT("NOT READY"));
}

void ANS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANS_PlayerState, bIsReady);
}