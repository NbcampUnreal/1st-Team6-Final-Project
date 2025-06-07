#include "InteractionComponent.h"
#include "Interaction/InteractionInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Inventory UI/NS_InventoryHUD.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Camera/CameraComponent.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
// UI 위젯 갱신 (HUD를 통해 상호작용 대상 정보 전달)
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

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}
// 시야 위치 (캐릭터 기준 약간 위로)
FVector UInteractionComponent::GetViewLocation() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->GetActorLocation() + FVector(0, 0, 50.f) : FVector::ZeroVector;
}
// 시야 방향 (카메라 방향)
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
// 상호작용 대상 감지 (라인 트레이스 사용)
void UInteractionComponent::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
	
	// CameraComponent 기준으로 라인트레이스
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UCameraComponent* CameraComp = OwnerActor->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;

	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComp->GetForwardVector() * InteractionCheckDistance);
	// 플레이어가 바라보는 방향 확인
	float LookDirection = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		FHitResult TraceHit;
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor() && TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					// 새로운 대상 발견 시 처리
					FoundInteractable(TraceHit.GetActor());
					return;
				}
				// 같은 대상이면 그대로 유지
				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	// 감지 실패 시 처리
	NoInteractableFound();
}
// 새로운 상호작용 대상 발견
void UInteractionComponent::FoundInteractable(AActor* NewInteractable)
{
	// 상호작용 중이면 종료
	if (IsInteracting())
	{
		EndInteract();
	}
	// 이전 대상이 있다면 포커스 종료
	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	// 새로운 대상 등록 및 위젯 갱신
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	// 포커스 시작
	TargetInteractable->BeginFocus();
}
// 상호작용 대상이 없는 경우 처리
void UInteractionComponent::NoInteractableFound()
{
	// 상호작용 중이면 타이머 해제
	if (IsInteracting())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		// 포커스 종료
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}
		// 위젯 숨기기
		HUD->HideInteractionWidget();
		// 현재 대상 초기화
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}
// 상호작용 시작
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
	// 대상 감지 시도
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
// 상호작용 종료
void UInteractionComponent::EndInteract()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}
// 실제 상호작용 실행
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
// 서버에서 상호작용 처리
void UInteractionComponent::Interact_Server_Implementation(AActor* Target)
{
	if (Target && Target->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_Interact(Target, GetOwner());
	}
}

