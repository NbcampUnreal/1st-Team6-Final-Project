#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "NS_Trap.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_Trap : public AActor
{
    GENERATED_BODY()

public:
    ANS_Trap();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayTrapSound();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UBoxComponent* TriggerBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UStaticMeshComponent* TrapMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap")
    USoundCue* TrapSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap")
    bool bPlayOnce = true;

private:
    bool bHasPlayed = false;
};
