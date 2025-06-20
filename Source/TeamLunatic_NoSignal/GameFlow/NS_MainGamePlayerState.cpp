#include "GameFlow/NS_MainGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "NS_GameInstance.h"

ANS_MainGamePlayerState::ANS_MainGamePlayerState()
{
    // 기본 생성자
}

void ANS_MainGamePlayerState::LoadPlayerData()
{
	if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		int32 LoadingPlayerId = GetPlayerId();
		if (GameInstance->PlayerDataMap.Contains(LoadingPlayerId))
		{
			FNS_PlayerData& PlayerData = GameInstance->PlayerDataMap[LoadingPlayerId];
			PlayerModelPath = PlayerData.CharacterModelPath;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No player data found for PlayerId: %d"), LoadingPlayerId);
			PlayerModelPath = TEXT("/Game/Character/Blueprints/Character/BP_NS_Male1"); // 기본값 설정(남캐1)
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to UNS_GameInstance"));
	}
}

void ANS_MainGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_MainGamePlayerState, SelectedPawnClass);
    DOREPLIFETIME(ANS_MainGamePlayerState, PlayerIndex);
	DOREPLIFETIME(ANS_MainGamePlayerState, bIsAlive);
}
