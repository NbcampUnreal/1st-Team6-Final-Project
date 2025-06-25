// NS_StatueController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NS_StatueController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_StatueController : public AAIController
{
	GENERATED_BODY()

public:
	ANS_StatueController();

protected:
	// 비헤이비어 트리 에셋만 멤버 변수로 가집니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	// AI 퍼셉션 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

	// Blackboard와 BrainComponent 선언을 여기서 삭제했습니다.

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};