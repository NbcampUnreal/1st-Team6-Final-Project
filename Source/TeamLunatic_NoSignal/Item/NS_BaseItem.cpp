#include "Item/NS_BaseItem.h"

ANS_BaseItem::ANS_BaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANS_BaseItem::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item DataTable assigned on %s"), *GetName());
		return;
	}

	if (ItemData)
	{
		ItemType = ItemData->ItemType;
		WeaponType = ItemData->WeaponType;
		ItemName = ItemData->ItemTextData.ItemName;
		Weight = ItemData->ItemNumericData.Weight;
		GetItemSound = ItemData->ItemAssetData.UseSound;
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		Icon = ItemData->ItemAssetData.Icon;
	}

	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Name = ItemName;
	InstanceInteractableData.Action = FText::FromString("to pick up");
}

const FNS_ItemDataStruct* ANS_BaseItem::GetItemData() const
{
	if (!ItemsDataTable || ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블 또는 RowName 없음"));
		return nullptr;
	}

	return ItemsDataTable->FindRow<FNS_ItemDataStruct>(ItemDataRowName, TEXT(""));
}

// Called every frame
void ANS_BaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANS_BaseItem::OnUseItem()
{

}

void ANS_BaseItem::BeginFocus()
{
	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetRenderCustomDepth(true);
	}
}

void ANS_BaseItem::EndFocus()
{
	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetRenderCustomDepth(false);
	}
}

