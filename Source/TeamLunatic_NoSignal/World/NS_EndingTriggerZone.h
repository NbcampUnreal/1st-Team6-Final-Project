// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/WidgetComponent.h"
#include "NS_EndingTriggerZone.generated.h"

class UBoxComponent;
class ANS_PlayerCharacterBase;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_EndingTriggerZone : public AActor
{
    GENERATED_BODY()

public:
    ANS_EndingTriggerZone();

    UPROPERTY(VisibleAnywhere, Category = "UI")
    class UWidgetComponent* EndingStatusWidget;
protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void CheckGroupEndingCondition();

    void UpdateWidgetStatus(int32 NumPlayers, int32 NumItems);

    void UpdateEndingCountdownUI();

    void EndingConditionSatisfied();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_TriggerEnding(bool bGroupConditionMet);

    FTimerHandle EndingConditionTimerHandle;
    FTimerHandle CountdownUpdateTimerHandle;
    bool bIsEndingTimerRunning = false;
protected:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;

    UPROPERTY()
    TArray<ANS_PlayerCharacterBase*> OverlappingPlayers;

    float EndingCountdown = 0.f;
};