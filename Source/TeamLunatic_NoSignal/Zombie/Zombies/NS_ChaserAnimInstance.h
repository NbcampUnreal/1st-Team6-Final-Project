#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NS_ChaserAnimInstance.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ChaserAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool IsKneel = false;
};