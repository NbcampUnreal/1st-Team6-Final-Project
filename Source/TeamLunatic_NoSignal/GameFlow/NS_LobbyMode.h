#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NS_LobbyMode.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyMode : public AGameMode
{
    GENERATED_BODY()

public:
    ANS_LobbyMode(); // 생성자 선언
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;

protected:
    AActor* FindSpawnPointByIndex(int32 Index);
};
