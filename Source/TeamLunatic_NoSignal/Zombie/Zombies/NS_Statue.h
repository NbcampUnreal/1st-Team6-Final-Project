// NS_Statue.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_Statue.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_Statue : public ACharacter
{
	GENERATED_BODY()

public:
	ANS_Statue();

protected:

	virtual void BeginPlay() override;
};