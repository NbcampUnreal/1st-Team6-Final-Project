#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "NS_InteractionComponent.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class ANS_InventoryHUD;
class UNS_InventoryBaseItem;
class APickup;

// 상호작용 데이터를 저장하는 구조체
USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
		: CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{
	}

	// 현재 상호작용 가능한 액터
	UPROPERTY()
	AActor* CurrentInteractable;

	// 마지막 상호작용 확인 시간
	UPROPERTY()
	float LastInteractionCheckTime;
};

// 주변 아이템 정보를 저장하는 구조체
USTRUCT(BlueprintType)
struct FNearbyItemInfo
{
	GENERATED_BODY()

	FNearbyItemInfo()
		: ItemActor(nullptr), Item(nullptr), Quantity(0)
	{
	}

	FNearbyItemInfo(APickup* InItemActor, UNS_InventoryBaseItem* InItem, int32 InQuantity)
		: ItemActor(InItemActor), Item(InItem), Quantity(InQuantity)
	{
	}

	// 아이템 액터
	UPROPERTY()
	APickup* ItemActor;

	// 아이템 데이터
	UPROPERTY()
	UNS_InventoryBaseItem* Item;

	// 아이템 수량
	UPROPERTY()
	int32 Quantity;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNearbyItemsUpdated);

// 플레이어의 상호작용을 처리하는 컴포넌트
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMLUNATIC_NOSIGNAL_API UNS_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNS_InteractionComponent();

	// 상호작용 가능한 오브젝트를 확인
	void PerformInteractionCheck();
	// 상호작용 시작
	void BeginInteract();
	// 상호작용 종료
	void EndInteract();

	// 현재 상호작용 중인지 확인
	FORCEINLINE bool IsInteracting() const { return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Interaction); }

	// 현재 상호작용 가능한 액터를 가져옴
	TScriptInterface<class INS_InteractionInterface> GetCurrentInteractable() const { return TargetInteractable; }
	// 상호작용 위젯을 업데이트
	void UpdateInteractionWidget();
	// 인벤토리 위젯 오픈
	void ToggleInventoryMenu();

	// 주변 아이템 목록 가져오기
	TArray<FNearbyItemInfo> GetNearbyItems() const { return NearbyItems; }

	// 주변 아이템을 인벤토리로 이동
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickupNearbyItem(APickup* ItemActor);

	// 주변 아이템 목록이 업데이트될 때 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnNearbyItemsUpdated OnNearbyItemsUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// HUD에 대한 참조
	UPROPERTY()
	ANS_InventoryHUD* HUD;

	// 아이템 감지 범위
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ItemDetectionRadius = 300.0f;

	// 주변 아이템 감지 주기
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ItemDetectionFrequency = 0.5f;

	// 주변 아이템 감지 타이머 핸들
	FTimerHandle TimerHandle_ItemDetection;

	// 주변 아이템 목록
	TArray<FNearbyItemInfo> NearbyItems;

	// 주변 아이템 감지 함수
	void DetectNearbyItems();

private:
	// 상호작용 데이터
	FInteractionData InteractionData;

	// 상호작용 대상
	TScriptInterface<class INS_InteractionInterface> TargetInteractable;

	// 상호작용 확인 거리
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionCheckDistance = 225.0f;

	// 상호작용 확인 주기
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionCheckFrequency = 0.1f;

	// 상호작용 타이머 핸들
	FTimerHandle TimerHandle_Interaction;

	// 상호작용 가능한 액터를 찾았을 때 호출
	void FoundInteractable(AActor* NewInteractable);
	// 상호작용 가능한 액터를 찾지 못했을 때 호출
	void NoInteractableFound();
	// 상호작용 위젯을 안전하게 숨김
	void HideInteractionWidgetSafely();
	// 상호작용 실행
	void Interact();
	// 서버에서 상호작용을 처리하는 함수
	UFUNCTION(Server, Reliable)
	void Interact_Server(AActor* Target);

	// 플레이어의 시점 위치를 가져옴
	FVector GetViewLocation() const;
	// 플레이어의 시점 회전값을 가져옴
	FRotator GetViewRotation() const;
};