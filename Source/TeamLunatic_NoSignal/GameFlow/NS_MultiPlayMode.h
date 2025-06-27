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
    virtual void CheckAndSpawnZombies() override;
    virtual void CleanupDistantZombies() override;

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver = false;

    UPROPERTY()
    int32 ZombiesRemoved = 0;

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
};