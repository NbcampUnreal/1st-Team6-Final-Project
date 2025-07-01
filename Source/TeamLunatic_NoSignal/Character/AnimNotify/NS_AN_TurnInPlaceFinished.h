#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AN_TurnInPlaceFinished.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_AN_TurnInPlaceFinished : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    UNS_AN_TurnInPlaceFinished();
    
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
    
    virtual FString GetNotifyName_Implementation() const override;
};