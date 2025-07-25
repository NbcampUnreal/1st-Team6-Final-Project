#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/Interface/NS_InteractionInterface.h"
#include "NS_DeathBox.generated.h"

class UNS_InventoryBaseItem;
class UStaticMeshComponent;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_DeathBox : public AActor, public INS_InteractionInterface
{
	GENERATED_BODY()
	
public:	
	ANS_DeathBox();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Items")
	TArray<TObjectPtr<UNS_InventoryBaseItem>> StoredItems;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float DestroyDelay = 300.0f; // 5분 후 자동 삭제

public:
	// 아이템들을 상자에 저장
	UFUNCTION(BlueprintCallable)
	void StoreItems(const TArray<UNS_InventoryBaseItem*>& Items);

	// 인터랙션 인터페이스 구현
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void Interact_Implementation(AActor* InteractingActor) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FTimerHandle DestroyTimerHandle;
};