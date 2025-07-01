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

    // Flask 서버에 플레이어 로그인/로그아웃 알림 함수들
    void NotifyPlayerLogin();
    void NotifyPlayerLogout();

protected:
    AActor* FindSpawnPointByIndex(int32 Index);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> PawnClassesToSpawn;
};