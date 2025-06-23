#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AN_PickupFinished.generated.h"

/**
 * 무기 획득 애니메이션이 끝났을 때 호출되는 애니메이션 노티파이
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AN_PickupFinished : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    UNS_AN_PickupFinished();
    
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
    
    virtual FString GetNotifyName_Implementation() const override;
};