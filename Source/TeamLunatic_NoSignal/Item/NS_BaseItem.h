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

<<<<<<< HEAD
	UInventoryComponent* OwingInventory;
	
protected:
  virtual void BeginPlay();
  
=======
protected:
	virtual void BeginPlay() override;

>>>>>>> parent of 189edb63a ([Fix]인벤토리 컴포넌트, 픽업 수정 #7)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Confige")
	UDataTable* ItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Confige")
	FName ItemDataRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	EItemType ItemType;

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
	FText GetItemName() const { return ItemName; }
	float GetWeight() const { return Weight; }
	const FNS_ItemDataStruct* GetItemData() const;

	virtual void Tick(float DeltaTime) override;

	virtual void OnUseItem();

<<<<<<< HEAD
	virtual void BeginFocus();
	virtual void EndFocus();

protected:
	bool operator == (const FName& OtherID) const
	{
		return ItemDataRowName == OtherID;
	}
=======
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
>>>>>>> parent of 189edb63a ([Fix]인벤토리 컴포넌트, 픽업 수정 #7)
};
