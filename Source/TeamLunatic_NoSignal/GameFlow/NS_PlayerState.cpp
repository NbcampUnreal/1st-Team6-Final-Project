#include "NS_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "NS_LobbyMode.h"
#include "Kismet/GameplayStatics.h"
#include "NS_GameInstance.h"

ANS_PlayerState::ANS_PlayerState()
{
	bIsReady = false;
	bReplicates = true;
	UE_LOG(LogTemp, Warning, TEXT("[PlayerState] BeginPlay: %s"), *GetName());
}

void ANS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_PlayerState, bIsReady);
	DOREPLIFETIME(ANS_PlayerState, PlayerIndex);
}

void ANS_PlayerState::SetIsReady(bool bReady)
{
	if (HasAuthority())
	{
		if (bIsReady != bReady)
		{
			bIsReady = bReady;
	
		}

		if (UWorld* World = GetWorld())
		{
			if (AGameModeBase* GM = World->GetAuthGameMode())
			{
				if (ANS_LobbyMode* LobbyMode = Cast<ANS_LobbyMode>(GM))
				{
					UE_LOG(LogTemp, Warning, TEXT(" CheckAllPlayersReady 호출!"));
					LobbyMode->CheckAllPlayersReady();
				}
			}
		}
	}
}




void ANS_PlayerState::ServerSetIsReady_Implementation(bool bReady)
{
	SetIsReady(bReady);
}

void ANS_PlayerState::OnRep_IsReady()
{
	// ReadyUI 갱신
	if (UWorld* World = GetWorld())
	{
		if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(UGameplayStatics::GetGameInstance(World)))
		{
			if (GI->ReadyUIInstance)
			{
				GI->ReadyUIInstance->UpdatePlayerStatusList();
			}
		}
	}
}



void ANS_PlayerState::SetPlayerIndex(int32 Index)
{
	if (HasAuthority())
	{
		PlayerIndex = Index;
	}
}

void ANS_PlayerState::SetPlayerModelPath(const FString& ModelPath)
{
	PlayerModelPath = ModelPath;
}

void ANS_PlayerState::SavePlayerData()
{
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
