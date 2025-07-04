// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class ANS_InventoryHUD;

USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
		: CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{
	}

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMLUNATIC_NOSIGNAL_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

	void PerformInteractionCheck();
	void BeginInteract();
	void EndInteract();

	FORCEINLINE bool IsInteracting() const { return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Interaction); }

	TScriptInterface<class IInteractionInterface> GetCurrentInteractable() const { return TargetInteractable; }
	void UpdateInteractionWidget();
	void ToggleMenu();
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	ANS_InventoryHUD* HUD;
private:
	FInteractionData InteractionData;

	TScriptInterface<class IInteractionInterface> TargetInteractable;

	UPROPERTY(EditAnywhere)
	float InteractionCheckDistance = 225.0f;

	UPROPERTY(EditAnywhere)
	float InteractionCheckFrequency = 0.1f;

	FTimerHandle TimerHandle_Interaction;

	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void HideInteractionWidgetSafely();
	void Interact();
	UFUNCTION(Server, Reliable)
	void Interact_Server(AActor* Target);

	FVector GetViewLocation() const;
	FRotator GetViewRotation() const;
};