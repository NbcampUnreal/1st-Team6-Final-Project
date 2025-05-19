#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EGameModeType.h"  
#include "NS_GameInstance.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 게임 모드 설정
	void SetGameModeType(EGameModeType Type);
	EGameModeType GetGameModeType() const { return GameModeType; }


private:
	EGameModeType GameModeType = EGameModeType::SinglePlayMode;
};
