#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_PlayerController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANS_PlayerController();

protected:
	virtual void BeginPlay() override;
};
