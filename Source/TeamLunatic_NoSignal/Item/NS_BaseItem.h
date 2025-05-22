#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ItemDataStruct.h"
#include "Engine/DataTable.h"
#include "Interaction/InteractionInterface.h"
#include "NS_BaseItem.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ANS_BaseItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Confige")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Confige")
	FName ItemDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	USoundBase* GetItemSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	FInteractableData InstanceInteractableData;

public:	
	EItemType GetItemType() const { return ItemType; }
	EWeaponType GetWeaponType() const { return WeaponType; }
	FText GetItemName() const { return ItemName; }
	float GetWeight() const { return Weight; }
	const FNS_ItemDataStruct* GetItemData() const;

	virtual void Tick(float DeltaTime) override;

	virtual void OnUseItem();

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
};
