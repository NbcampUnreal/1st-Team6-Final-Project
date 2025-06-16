// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_EndingTriggerZone.generated.h"

class UBoxComponent;
class ANS_PlayerCharacterBase;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_EndingTriggerZone : public AActor
{
    GENERATED_BODY()

public:
    ANS_EndingTriggerZone();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void CheckGroupEndingCondition();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_TriggerEnding();

protected:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;

    UPROPERTY()
    TArray<ANS_PlayerCharacterBase*> OverlappingPlayers;
};