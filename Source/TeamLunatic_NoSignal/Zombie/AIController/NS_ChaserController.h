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

    // 강제로 플레이어 위치 요청 (좌표 이동용) 
    UFUNCTION(BlueprintCallable)
    void RequestPlayerLocation();

    //  강제 추적 (시야/청각 외부에서 직접 호출) 
    UFUNCTION(BlueprintCallable)
    void SetChaseTarget(AActor* Target, float Duration = 60.0f);

    // 추적 종료 및 쿨다운 시작 
    void ResetChase();


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 감지 처리 (시야/청각) 
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

   
    // 지속 데미지 루프 시작 / 정지 / 실행 
    void StartDamageLoop(AActor* Target);
    void StopDamageLoop();
    void ApplyDamageToTarget();



    // AI 요소 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBlackboardComponent* BlackboardComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAIPerceptionComponent* PerceptionComp;

    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    // 외부 할당 BT / BB Asset 
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBlackboardData* BlackboardAsset;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    // 추적 타이머 및 쿨다운 타이머 
    FTimerHandle ChaseResetTimerHandle;
    FTimerHandle CooldownTimerHandle;
    FTimerHandle PatrolWaitTimerHandle;
    // 지속 데미지용
    FTimerHandle DamageTimerHandle;
    AActor* DamageTarget = nullptr;
    bool bIsDealingDamage = false;
};
