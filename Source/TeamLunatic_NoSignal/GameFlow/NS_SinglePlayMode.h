#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "NS_GameModeBase.h"
#include "NS_SinglePlayMode.generated.h"

class AActor;
class APawn;
class ANS_PlayerCharacterBase;

// 탈출 루트 구분
UENUM(BlueprintType)
enum class EEscapeRoute : uint8
{
    None,
    Car,
    Radio
};

// 탈출 시도 정보
USTRUCT(BlueprintType)
struct FEscapeAttemptInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    ANS_PlayerCharacterBase* Player = nullptr;

    UPROPERTY(BlueprintReadWrite)
    FName EscapeTargetTag = NAME_None;
};

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_SinglePlayMode : public ANS_GameModeBase
{
    GENERATED_BODY()

protected:
    ANS_SinglePlayMode();

    virtual void BeginPlay() override;
    
    UFUNCTION(BlueprintCallable, Category = "Location")
    virtual FVector GetPlayerLocation_Implementation() const override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

    virtual void OnPlayerCharacterDied_Implementation(ANS_PlayerCharacterBase* DeadCharacter) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    TArray<TSubclassOf<APawn>> SinglePlayPawnClasses; 


private:

    bool bIsGameOver = false;
    EEscapeRoute CurrentEscapeRoute = EEscapeRoute::None;
};