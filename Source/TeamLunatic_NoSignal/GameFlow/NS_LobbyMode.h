#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NS_LobbyMode.generated.h"

class APawn; // Pawn 클래스에 대한 전방 선언 추가

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_LobbyMode : public AGameMode
{
    GENERATED_BODY()

public:
    ANS_LobbyMode();
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    virtual void BeginPlay() override;
    void CheckAllPlayersReady();

protected:
    AActor* FindSpawnPointByIndex(int32 Index);

    // Lobby에서 임시 폰을 스폰하기 위한 배열 선언
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> PawnClassesToSpawn;
};