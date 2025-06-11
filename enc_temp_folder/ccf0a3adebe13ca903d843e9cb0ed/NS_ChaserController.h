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

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    void ResetChase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBlackboardComponent* BlackboardComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAIPerceptionComponent* PerceptionComp;

    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    FTimerHandle ChaseResetTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBlackboardData* BlackboardAsset;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    FTimerHandle CooldownTimerHandle;

};