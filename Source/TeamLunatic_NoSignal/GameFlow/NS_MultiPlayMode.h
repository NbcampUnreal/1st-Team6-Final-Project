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
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    UFUNCTION()
    void OptimizedMultiplaySpawnCheck();

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver = false;

protected:
    virtual void BeginPlay() override;
    FVector GetRandomPlayerLocation() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> MainGamePawnClassesToSpawn;

    UPROPERTY()
    TArray<TSubclassOf<APawn>> AvailablePawnsToSpawn;

private:
    FTimerHandle PlayerCountCheckTimer;
    void CheckPlayerCountAndEndSession();

    // 캐릭터 중복 방지
    UPROPERTY()
    TArray<int32> UsedPawnIndices;

    TSubclassOf<APawn> GetUniqueRandomPawnClass();
    void ReleasePawnIndex(APlayerController* ExitingPlayer);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ResetCharacterDuplicationSystem();
};
