#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NS_MainGamePlayerState.generated.h"

class APawn;
class NS_UIManagerClass;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_MainGamePlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ANS_MainGamePlayerState();

    // 선택한 캐릭터 클래스 (스폰 시 사용) 
    UPROPERTY(BlueprintReadWrite, Replicated)
    TSubclassOf<APawn> SelectedPawnClass;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_IsAlive)
    bool bIsAlive = true; // 기본값을 true로 설정합니다.

    // bIsAlive 변수가 복제되었을 때 호출될 함수입니다.
    UFUNCTION()
    void OnRep_IsAlive();
    
private:
};
