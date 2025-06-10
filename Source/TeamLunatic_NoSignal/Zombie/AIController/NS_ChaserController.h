// ChaserZombieController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "NS_ChaserController.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API AChaserZombieController : public AAIController
{
    GENERATED_BODY()

public:
    AChaserZombieController();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UAIPerceptionComponent* PerceptionComp;

    UAISenseConfig_Sight* SightConfig;
    UAISenseConfig_Hearing* HearingConfig;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
