#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NS_MoveStopandStartNotify.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_MoveStopandStartNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 애니메이션 시작 부분에서 호출
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
							 UAnimSequenceBase* Animation,
							 float TotalDuration) override;

	// 애니메이션 종료 부분에서 호출
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
						   UAnimSequenceBase* Animation) override;
};
