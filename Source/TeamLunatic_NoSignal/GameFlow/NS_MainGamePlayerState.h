#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NS_MainGamePlayerState.generated.h"

class APawn;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainGamePlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ANS_MainGamePlayerState();

    // 선택한 캐릭터 클래스 (스폰 시 사용) 
    UPROPERTY(BlueprintReadWrite, Replicated)
    TSubclassOf<APawn> SelectedPawnClass;

    // UI 슬롯 인덱스 
    UPROPERTY(BlueprintReadWrite, Replicated)
    int32 PlayerIndex = -1;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
