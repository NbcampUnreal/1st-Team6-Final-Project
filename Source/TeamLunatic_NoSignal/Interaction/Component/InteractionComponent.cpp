#include "InteractionComponent.h"
#include "Interaction/InteractionInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Inventory UI/NS_InventoryHUD.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Character/NS_PlayerCharacterBase.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::UpdateInteractionWidget()
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void UInteractionComponent::ToggleMenu()
{
	HUD->ToggleMenu();
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ANS_InventoryHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ÀÏÁ¤ ½Ã°£¸¶´Ù PerformInteractionCheck() ½ÇÇà
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

FVector UInteractionComponent::GetViewLocation() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->GetActorLocation() + FVector(0, 0, 50.f) : FVector::ZeroVector;
}

FRotator UInteractionComponent::GetViewRotation() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (AController* Controller = Pawn->GetController())
		{
			return Controller->GetControlRotation();
		}
	}
	return GetOwner()->GetActorRotation();
}

void UInteractionComponent::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart = GetViewLocation();
	FVector TraceEnd = TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance);

	float LookDirection = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.f, 0, 2.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		FHitResult TraceHit;
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor() && TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	NoInteractableFound();
}

void UInteractionComponent::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void UInteractionComponent::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void UInteractionComponent::BeginInteract()
{
	// 현재 캐릭터가 이미 상호작용 중이라면 무시
	if (const ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner()))
	{
		if (PlayerCharacter->IsPickUp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Already picking up item"));
			return;
		}
	}

	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable && IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->BeginInteract();

		if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
		{
			Interact();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interaction, this, &UInteractionComponent::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
		}
	}
}

void UInteractionComponent::EndInteract()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void UInteractionComponent::Interact()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		if (!GetOwner()->HasAuthority())
		{
			// RPC로 서버에 요청
			Interact_Server(Cast<AActor>(TargetInteractable.GetObject()));
		}
		else
		{
			// 서버에서 Execute 방식으로 안전하게 호출
			if (TargetInteractable.GetObject()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				IInteractionInterface::Execute_Interact(TargetInteractable.GetObject(), GetOwner());
			}
		}
	}
}

void UInteractionComponent::Interact_Server_Implementation(AActor* Target)
{
	if (Target && Target->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_Interact(Target, GetOwner());
	}
}

