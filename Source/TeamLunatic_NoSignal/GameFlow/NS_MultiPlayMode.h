#pragma once
#include "CoreMinimal.h"
#include "NS_GameModeBase.h"
#include "NS_MultiPlayMode.generated.h"

class ANS_PlayerCharacterBase;
class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MultiPlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

public:
    ANS_MultiPlayMode();
    virtual FVector GetPlayerLocation_Implementation() const override;
    virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;
    void PostLogin(APlayerController* NewPlayer);
    virtual void Logout(AController* Exiting) override;  // 플레이어 로그아웃 시 호출
    virtual void CheckAndSpawnZombies() override;
    virtual void CleanupDistantZombies() override;

    // 최적화된 스폰 체크 함수
    UFUNCTION()
    void OptimizedMultiplaySpawnCheck();

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver = false;

    UPROPERTY()
    int32 ZombiesRemoved = 0;

    // 로딩 동기화 시스템
    UPROPERTY()
    TArray<APlayerController*> LoadingCompletedPlayers;

    UFUNCTION()
    void OnPlayerLoadingComplete(APlayerController* Player);

    UFUNCTION()
    void CheckAllPlayersLoadingComplete();

protected:
    virtual void BeginPlay() override;
    FVector GetRandomPlayerLocation() const;
    TArray<AANS_ZombieSpawner*> FindSuitableSpawnersForMultiplay(const FVector& CurrentPlayerLocation, const TArray<FVector>& AllPlayerLocations);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> MainGamePawnClassesToSpawn;

    UPROPERTY()
    TArray<TSubclassOf<APawn>> AvailablePawnsToSpawn;

private:
    FTimerHandle PlayerCountCheckTimer;
    void CheckPlayerCountAndEndSession();

    // Flask 서버에 플레이어 로그인/로그아웃 알림 함수들
    void NotifyPlayerLogin();
    void NotifyPlayerLogout();
};