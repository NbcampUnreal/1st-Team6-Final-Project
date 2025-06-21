// NS_GameModeBase.h (이전과 동일)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h" 

#include "NS_GameModeBase.generated.h"

UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
	
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const; 
    virtual FVector GetPlayerLocation_Implementation() const; 
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    void OnPlayerCharacterDied(class ANS_PlayerCharacterBase* DeadCharacter);
    virtual void OnPlayerCharacterDied_Implementation(class ANS_PlayerCharacterBase* DeadCharacter) PURE_VIRTUAL(ANS_GameModeBase::OnPlayerCharacterDied, );
    
    void CheckAndSpawnZombies();
    void SpawnZombieAtPoint(AANS_ZombieSpawner* SpawnPoint); 

    UFUNCTION()
    void OnZombieDestroyed(AActor* DestroyedActor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie")
    int32 CurrentZombieCount = 0;

    UPROPERTY(EditAnywhere, Category = "Zombie")
    int32 MaxZombieCount = 150;
    
    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_BasicZombie> BasicZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_FatZombie> FatZombieClass;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie")
    TSubclassOf<class ANS_RunnerZombie> RunnerZombieClass;

    TArray<AANS_ZombieSpawner*> ZombieSpawnPoints;

    FTimerHandle ZombieSpawnTimer;
};