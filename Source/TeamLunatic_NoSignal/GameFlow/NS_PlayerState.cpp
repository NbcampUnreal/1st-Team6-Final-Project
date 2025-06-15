#include "NS_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "NS_GameInstance.h"

ANS_PlayerState::ANS_PlayerState()
{
	bIsReady = false;
	bReplicates = true;
	UE_LOG(LogTemp, Warning, TEXT("📙 [PlayerState] BeginPlay: %s"), *GetName());
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
	DOREPLIFETIME(ANS_PlayerState, PlayerIndex);
}

void ANS_PlayerState::SetPlayerIndex(int32 Index)
{
	if (HasAuthority())
	{
		PlayerIndex = Index;
		UE_LOG(LogTemp, Log, TEXT("[PlayerState] PlayerIndex set to %d"), PlayerIndex);
	}
}

void ANS_PlayerState::SetPlayerModelPath(const FString& ModelPath)
{
	PlayerModelPath = ModelPath;
}

void ANS_PlayerState::SavePlayerData()
{
	//게임 인스턴스에 저장
	if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		int32 SavingPlayerId = GetPlayerId();

		if (!GameInstance->PlayerDataMap.Contains(SavingPlayerId))
		{
			FNS_PlayerData NewPlayerData;
			NewPlayerData.CharacterModelPath = TEXT(""); // 기본값 설정
			GameInstance->PlayerDataMap.Add(SavingPlayerId, NewPlayerData);
		}

		FNS_PlayerData& PlayerData = GameInstance->PlayerDataMap[SavingPlayerId];

		PlayerData.CharacterModelPath = PlayerModelPath;

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to UNS_GameInstance"));
	}
}

void ANS_PlayerState::LoadPlayerData()
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
