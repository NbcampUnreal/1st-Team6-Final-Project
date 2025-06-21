#include "GameFlow/NS_MainGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/NS_UIManager.h"
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

void ANS_MainGamePlayerState::OnRep_IsAlive()
{
    if (!bIsAlive)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayerState] OnRep_IsAlive: 사망 상태 감지. UI 표시 시도."));

        if (APlayerController* PC = GetPlayerController())
        {
            if (PC->IsLocalController())
            {
                UE_LOG(LogTemp, Warning, TEXT("PC는 로컬 컨트롤러입니다."));

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
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PC는 로컬 컨트롤러가 아님"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GetPlayerController() 실패"));
        }
    }
}
