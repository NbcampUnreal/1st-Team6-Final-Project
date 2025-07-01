#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Zombie/ZombieSpawner/NS_ZombieSpawner.h"
#include "ZOmbie/NS_ZombieBase.h"
#include "NS_GameModeBase.generated.h"

UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANS_GameModeBase();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const;
    virtual FVector GetPlayerLocation_Implementation() const; 

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    void OnPlayerCharacterDied(class ANS_PlayerCharacterBase* DeadCharacter);
    virtual void OnPlayerCharacterDied_Implementation(class ANS_PlayerCharacterBase* DeadCharacter);
    // 캐시된 좀비 리스트 (약한 참조 사용)

    // 플레이어 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Spawning")
    int32 PlayerCount = 1;

};
