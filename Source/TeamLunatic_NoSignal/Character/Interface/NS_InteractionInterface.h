// NS_InteractionInterface.h - 상호작용 가능한 객체를 위한 인터페이스

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NS_InteractionInterface.generated.h"

/**
 * 상호작용 가능한 객체의 유형을 정의하는 열거형
 */
UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	None UMETA(DisplayName = "None"),
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"),
	Toggle UMETA(DisplayName = "Toggle"),
	Container UMETA(DisplayName = "Container")
};

/**
 * 상호작용 가능한 객체의 데이터를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() : 
		InteractableType(EInteractableType::None),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.0f)
	{
	};

	// 상호작용 객체의 유형
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	EInteractableType InteractableType;

	// 상호작용 객체의 이름
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	FText Name;

	// 상호작용 시 표시할 액션 텍스트
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	FText Action;

	// 아이템의 수량 (해당되는 경우)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	int32 Quantity;

	// 상호작용 완료까지 필요한 시간 (0이면 즉시 완료)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionDuration;
};

// 인터페이스 선언
UINTERFACE(MinimalAPI)
class UNS_InteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 상호작용 가능한 객체가 구현해야 하는 인터페이스
 */
class TEAMLUNATIC_NOSIGNAL_API INS_InteractionInterface
{
	GENERATED_BODY()

public:
	// 플레이어가 이 객체에 포커스를 시작할 때 호출
	virtual void BeginFocus();
	
	// 플레이어가 이 객체에서 포커스를 해제할 때 호출
	virtual void EndFocus();
	
	// 플레이어가 이 객체와 상호작용을 시작할 때 호출
	virtual void BeginInteract();
	
	// 플레이어가 이 객체와 상호작용을 종료할 때 호출
	virtual void EndInteract();
	
	// 상호작용이 완료될 때 호출
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* InteractingActor);

	// 이 객체의 상호작용 데이터
	FInteractableData InteractableData;
};