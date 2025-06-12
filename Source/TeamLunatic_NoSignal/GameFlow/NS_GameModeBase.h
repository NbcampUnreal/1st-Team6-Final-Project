// NS_GameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NS_GameModeBase.generated.h"

UCLASS(Abstract)
class TEAMLUNATIC_NOSIGNAL_API ANS_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    // 공통 인터페이스: 플레이어 위치 반환 
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Common")
    FVector GetPlayerLocation() const;
    virtual FVector GetPlayerLocation_Implementation() const PURE_VIRTUAL(ANS_GameModeBase::GetPlayerLocation, return FVector::ZeroVector;);
};
