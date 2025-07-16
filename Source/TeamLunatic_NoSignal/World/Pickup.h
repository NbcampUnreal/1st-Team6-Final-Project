#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "Net/UnrealNetwork.h"
#include "Item/NS_ItemDataStruct.h"
#include "Pickup.generated.h"

class UDataTable;
class UNS_InventoryBaseItem;
class ANS_PlayerCharacterBase;

/**
 * 월드에 배치되는 아이템 액터 클래스
 * 플레이어가 상호작용하여 인벤토리에 추가할 수 있는 아이템을 표현
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API APickup : public AActor, public INS_InteractionInterface
{
	GENERATED_BODY()

public:
	/** 생성자 */
	APickup();

	/**
	 * 아이템 초기화 함수 (에디터에서 배치된 아이템용)
	 * @param BaseClass 아이템 클래스
	 * @param InQuantity 아이템 수량
	 */
	void InitializePickup(const TSubclassOf<UNS_InventoryBaseItem> BaseClass, const int32 InQuantity);
	
	/**
	 * 아이템 초기화 함수 (플레이어가 버린 아이템용)
	 * @param ItemToDrop 버릴 아이템
	 * @param InQuantity 아이템 수량
	 */
	void InitializeDrop(UNS_InventoryBaseItem* ItemToDrop, const int32 InQuantity);

	/** 복제되는 아이템 데이터 */
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedItemData)
	FNS_ItemDataStruct ReplicatedItemData;

	/** 복제된 아이템 데이터가 업데이트될 때 호출 */
	UFUNCTION()
	void OnRep_ReplicatedItemData();

	/** 아이템 데이터 반환 */
	FORCEINLINE UNS_InventoryBaseItem* GetItemData() { return ItemReference; };
	
	/** 아이템 반환 */
	FORCEINLINE UNS_InventoryBaseItem* GetItem() { return ItemReference; };
	
	/** 아이템 수량 반환 */
	FORCEINLINE int32 GetQuantity() const { return ItemQuantity; };

	//~ Begin INS_InteractionInterface
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	//~ End INS_InteractionInterface

	/**
	 * 플레이어가 아이템을 획득
	 * @param Taker 아이템을 획득하는 플레이어
	 */
	void TakePickup(ANS_PlayerCharacterBase* Taker);
	
	/** 서버에서 아이템 획득 처리 */
	UFUNCTION(Server, Reliable)
	void Server_TakePickup(AActor* InteractingActor);

	/** 아이템 메시 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	/** 아이템 데이터 테이블 */
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	UDataTable* ItemDataTable;

	/** 아이템 ID */
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	FName DesiredItemID;

	/** 아이템 참조 */
	UPROPERTY(VisibleAnywhere, Category = "Pickup | ItemReference")
	UNS_InventoryBaseItem* ItemReference;

	/** 아이템 수량 */
	UPROPERTY(EditInstanceOnly, Category = "Pickup | ItemInitialization")
	int32 ItemQuantity;

	/** 상호작용 데이터 인스턴스 */
	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;

protected:
	virtual void BeginPlay() override;
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	
	/** 상호작용 데이터 업데이트 */
	void UpdateInteractableData();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};