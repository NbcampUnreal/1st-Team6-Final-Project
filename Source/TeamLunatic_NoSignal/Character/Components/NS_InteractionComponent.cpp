#include "Character/Components/NS_InteractionComponent.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Inventory UI/NS_InventoryHUD.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory UI/Interaction/NS_InteractionWidget.h"
#include "Camera/CameraComponent.h"
#include "World/Pickup.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/Components/NS_InventoryComponent.h"
#include "Kismet/GameplayStatics.h"


UNS_InteractionComponent::UNS_InteractionComponent()
{
	// 이 컴포넌트가 매 프레임 업데이트되도록 설정합니다.
	PrimaryComponentTick.bCanEverTick = true;
}

// 상호작용 위젯을 업데이트합니다.
void UNS_InteractionComponent::UpdateInteractionWidget()
{
	// 대상 상호작용 객체가 유효한지 확인합니다.
	if (IsValid(TargetInteractable.GetObject()))
	{
		// HUD의 상호작용 위젯을 대상의 데이터로 업데이트합니다.
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

// 인벤토리메뉴 오픈
void UNS_InteractionComponent::ToggleInventoryMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleInventoryMenu 호출됨"));
    
    // HUD가 유효한지 확인하고, 없으면 다시 가져옵니다.
	if (!HUD)
	{
	    UE_LOG(LogTemp, Warning, TEXT("HUD가 유효하지 않음, 다시 가져오기 시도"));
		APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
		if (PC)
		{
			if (PC->IsLocalController())
			{
				AHUD* BaseHUD = PC->GetHUD();
				if (BaseHUD)
				{
				    UE_LOG(LogTemp, Warning, TEXT("BaseHUD 클래스: %s"), *BaseHUD->GetClass()->GetName());
					HUD = Cast<ANS_InventoryHUD>(BaseHUD);
					if (HUD)
					{
					    UE_LOG(LogTemp, Warning, TEXT("HUD 캐스팅 성공"));
					}
					else
					{
					    UE_LOG(LogTemp, Error, TEXT("HUD 캐스팅 실패! BaseHUD가 ANS_InventoryHUD 클래스가 아닙니다."));
					}
				}
				else
				{
				    UE_LOG(LogTemp, Error, TEXT("BaseHUD가 유효하지 않음"));
				}
			}
		}
	}
	
	// HUD가 유효하면 인벤토리 위젯을 오픈합니다.
	if (HUD)
	{
	    UE_LOG(LogTemp, Warning, TEXT("HUD가 유효함, 인벤토리 위젯 오픈 시도"));
		// 인벤토리 위젯을 열고 주변 아이템 목록을 업데이트합니다.
		HUD->OpenInventoryWidget();
		
		// 주변 아이템 감지 함수를 즉시 호출하여 최신 상태를 표시합니다.
		DetectNearbyItems();
	}
	else
	{
	    UE_LOG(LogTemp, Error, TEXT("HUD가 여전히 유효하지 않음, 인벤토리 위젯 오픈 실패"));
	}
}

// 주변 아이템 감지 함수
void UNS_InteractionComponent::DetectNearbyItems()
{
	// 이전 목록 지우기
	NearbyItems.Empty();
	
	// 오너가 유효한지 확인
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// 오너 위치 가져오기
	FVector OwnerLocation = Owner->GetActorLocation();
	
	// 주변에 있는 모든 Pickup 액터 검색
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickup::StaticClass(), FoundActors);
	
	// 감지 범위 내의 아이템만 필터링
	for (AActor* Actor : FoundActors)
	{
		APickup* Pickup = Cast<APickup>(Actor);
		if (!Pickup) continue;
		
		// 거리 계산
		float Distance = FVector::Dist(OwnerLocation, Pickup->GetActorLocation());
		if (Distance <= ItemDetectionRadius)
		{
			// 아이템 정보 가져오기
			UNS_InventoryBaseItem* Item = Pickup->GetItem();
			int32 Quantity = Pickup->GetQuantity();
			
			if (Item)
			{
				// 주변 아이템 목록에 추가
				NearbyItems.Add(FNearbyItemInfo(Pickup, Item, Quantity));
			}
		}
	}
	
	// 주변 아이템 목록이 업데이트되었음을 알림
	OnNearbyItemsUpdated.Broadcast();
}

// 주변 아이템을 인벤토리로 이동
void UNS_InteractionComponent::PickupNearbyItem(APickup* ItemActor)
{
	// 오너가 유효한지 확인
	ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner());
	if (!PlayerCharacter) return;
	
	// 인벤토리 컴포넌트 확인
	UNS_InventoryComponent* InventoryComp = PlayerCharacter->GetInventory();
	if (!InventoryComp) return;
	
	// 아이템 액터가 유효한지 확인
	if (!ItemActor) return;
	
	// 아이템 정보 가져오기
	UNS_InventoryBaseItem* Item = ItemActor->GetItem();
	int32 Quantity = ItemActor->GetQuantity();
	
	if (!Item) return;
	
	// 서버에서 실행해야 하는 로직이므로 상호작용 인터페이스를 통해 처리
	if (ItemActor->GetClass()->ImplementsInterface(UNS_InteractionInterface::StaticClass()))
	{
		// 상호작용 시작
		ItemActor->BeginInteract();
		
		// 상호작용 실행
		INS_InteractionInterface::Execute_Interact(ItemActor, PlayerCharacter);
		
		// 상호작용 종료
		ItemActor->EndInteract();
	}
	
	// 주변 아이템 목록 업데이트
	DetectNearbyItems();
}

// 게임이 시작될 때 호출됩니다.
void UNS_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 오너의 컨트롤러를 가져와 로컬 플레이어 컨트롤러인지 확인합니다.
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (PC && PC->IsLocalController())
	{
		// HUD를 가져와 멤버 변수에 저장합니다.
		HUD = Cast<ANS_InventoryHUD>(PC->GetHUD());
		
		if (!HUD)
		{
			// HUD가 아직 생성되지 않았을 수 있으므로 약간의 지연 후 다시 시도합니다.
			FTimerHandle TimerHandle_RetryGetHUD;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RetryGetHUD, [this, PC]()
			{
				if (PC && PC->IsLocalController())
				{
					HUD = Cast<ANS_InventoryHUD>(PC->GetHUD());
				}
			}, 1.0f, false);
		}
	}
	
	// 주변 아이템 감지 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_ItemDetection,
		this,
		&UNS_InteractionComponent::DetectNearbyItems,
		ItemDetectionFrequency,
		true);
}

// 매 프레임 호출됩니다.
void UNS_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 일정 주기마다 상호작용 확인을 수행합니다.
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

// 플레이어의 시점 위치를 가져옵니다.
FVector UNS_InteractionComponent::GetViewLocation() const
{
	const AActor* Owner = GetOwner();
	// 오너의 위치를 기준으로 약간 위쪽 위치를 반환합니다.
	return Owner ? Owner->GetActorLocation() + FVector(0, 0, 50.f) : FVector::ZeroVector;
}

// 플레이어의 시점 회전값을 가져옵니다.
FRotator UNS_InteractionComponent::GetViewRotation() const
{
	// 오너가 폰인 경우 컨트롤러의 회전값을 사용합니다.
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (AController* Controller = Pawn->GetController())
		{
			return Controller->GetControlRotation();
		}
	}
	// 그렇지 않으면 오너의 액터 회전값을 사용합니다.
	return GetOwner()->GetActorRotation();
}

// 상호작용 가능한 액터를 확인합니다.
void UNS_InteractionComponent::PerformInteractionCheck()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC)
	{
		return;
	}
	// 마지막 확인 시간을 현재 시간으로 업데이트합니다.
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
	
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	// 카메라 컴포넌트를 기준으로 라인 트레이스를 수행합니다.
	UCameraComponent* CameraComp = OwnerActor->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;

	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComp->GetForwardVector() * InteractionCheckDistance);
	
	float LookDirection = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		FHitResult TraceHit;
		// 라인 트레이스를 통해 상호작용 가능한 액터를 찾습니다.
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			// 히트된 액터가 상호작용 인터페이스를 구현했는지 확인합니다.
			if (TraceHit.GetActor() && TraceHit.GetActor()->GetClass()->ImplementsInterface(UNS_InteractionInterface::StaticClass()))
			{
				// 현재 상호작용 대상과 다른 경우
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					// 새로운 상호작용 대상을 설정합니다.
					FoundInteractable(TraceHit.GetActor());
					return;
				}
				// 현재 상호작용 대상과 같은 경우
				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	// 상호작용 가능한 액터를 찾지 못한 경우
	NoInteractableFound();
}

// 새로운 상호작용 대상을 찾았을 때 호출됩니다.
void UNS_InteractionComponent::FoundInteractable(AActor* NewInteractable)
{
	// 현재 상호작용 중이라면 종료합니다.
	if (IsInteracting())
	{
		EndInteract();
	}
	// 이전에 상호작용 대상이 있었다면 포커스를 종료합니다.
	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	// 새로운 상호작용 대상을 설정하고 위젯을 업데이트합니다.
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;
	
	if (TargetInteractable.GetObject())
	{
		const FInteractableData& Data = TargetInteractable->InteractableData;
	}

	if (HUD)
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
	
	// 새로운 대상에 포커스를 시작합니다.
	TargetInteractable->BeginFocus();
}

// 상호작용 가능한 액터를 찾지 못했을 때 호출됩니다.
void UNS_InteractionComponent::NoInteractableFound()
{
	// 상호작용 중이라면 타이머를 해제합니다.
	if (IsInteracting())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		// 포커스를 종료하고 위젯을 숨깁니다.
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}
		
		HideInteractionWidgetSafely();

		// 현재 상호작용 대상을 초기화합니다.
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

// 상호작용 위젯을 안전하게 숨깁니다.
void UNS_InteractionComponent::HideInteractionWidgetSafely()
{
	if (GetNetMode() == NM_DedicatedServer) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC || !PC->IsLocalController()) return;

	ANS_InventoryHUD* MyHUD = Cast<ANS_InventoryHUD>(PC->GetHUD());
	if (!IsValid(MyHUD)) return;

	if (IsValid(MyHUD->GetInteractionWidget()))
	{
		MyHUD->HideInteractionWidget();
	}
}

// 상호작용을 시작합니다.
void UNS_InteractionComponent::BeginInteract()
{
	if (const ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner()))
	{
		// 플레이어가 아이템을 줍는 중이거나 애니메이션 변경 중이면 상호작용을 막습니다.
		if (PlayerCharacter->IsPickUp || PlayerCharacter->IsChangeAnim)
		{
			return;
		}
	}
	
	// 상호작용 대상을 확인합니다.
	PerformInteractionCheck();

	// 상호작용 대상이 유효하면 상호작용을 시작합니다.
	if (InteractionData.CurrentInteractable && IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->BeginInteract();

		// 상호작용 시간이 거의 0이면 즉시 실행하고, 그렇지 않으면 타이머를 설정합니다.
		if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
		{
			Interact();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interaction, this, &UNS_InteractionComponent::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
		}
	}
}

// 상호작용을 종료합니다.
void UNS_InteractionComponent::EndInteract()
{
	// 상호작용 타이머를 해제합니다.
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);

	// 대상이 유효하면 상호작용 종료를 호출합니다.
	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}

	// 위젯을 안전하게 숨깁니다.
	HideInteractionWidgetSafely();
}

// 실제 상호작용을 실행합니다.
void UNS_InteractionComponent::Interact()
{
	// 상호작용 타이머를 해제합니다.
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interaction);

	// 대상이 유효하면 서버에 상호작용을 요청하거나 직접 실행합니다.
	if (IsValid(TargetInteractable.GetObject()))
	{
		if (!GetOwner()->HasAuthority())
		{
			// 클라이언트이면 서버에 RPC로 요청합니다.
			Interact_Server(Cast<AActor>(TargetInteractable.GetObject()));
		}
		else
		{
			// 서버이면 직접 실행합니다.
			if (TargetInteractable.GetObject()->GetClass()->ImplementsInterface(UNS_InteractionInterface::StaticClass()))
			{
				INS_InteractionInterface::Execute_Interact(TargetInteractable.GetObject(), GetOwner());
			}
		}
	}
}

// 서버에서 상호작용을 처리합니다.
void UNS_InteractionComponent::Interact_Server_Implementation(AActor* Target)
{
	// 대상이 유효하고 인터페이스를 구현했다면 상호작용을 실행합니다.
	if (Target && Target->GetClass()->ImplementsInterface(UNS_InteractionInterface::StaticClass()))
	{
		INS_InteractionInterface::Execute_Interact(Target, GetOwner());
	}
}

