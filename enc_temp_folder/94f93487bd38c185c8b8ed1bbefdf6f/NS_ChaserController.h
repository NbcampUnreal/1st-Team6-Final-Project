#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NS_ChaserController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ChaserController : public AAIController
{
    GENERATED_BODY()

public:
    ANS_ChaserController();

    UFUNCTION(BlueprintCallable)
    void RequestPlayerLocation();

    UFUNCTION(BlueprintCallable)
    void SetChaseTarget(AActor* Target, float Duration = 60.0f);

protected:
    virtual void BeginPlay() override;

    /** 감지 처리 */
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    /** 추적 종료 처리 */
    void ResetChase();

    /** Blackboard + Perception */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBlackboardComponent* BlackboardComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAIPerceptionComponent* PerceptionComp;

    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    /** 외부 할당 BT / BB Asset */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBlackboardData* BlackboardAsset;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    /** 추적 제한 / 쿨다운 타이머 */
    FTimerHandle ChaseResetTimerHandle;
    FTimerHandle CooldownTimerHandle;
};
