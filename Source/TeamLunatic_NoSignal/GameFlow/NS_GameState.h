#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/DirectionalLight.h"
#include "NS_GameState.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	ANS_GameState();
protected:
	UPROPERTY(VisibleAnywhere, Category = "DayNight")
	ADirectionalLight* DirectionalLight = nullptr;

	UPROPERTY(EditAnywhere, Category = "DayNight")
	float RotationSpeedDegreesPerSec = 6.0f; // 

private:
	float TimeAccumulator = 0.0f;
};
