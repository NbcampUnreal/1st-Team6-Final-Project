#include "Item/NS_BaseItem.h"
#include "Inventory/InventoryComponent.h"



ANS_BaseItem::ANS_BaseItem() : bisCopy(false), bisPickup(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
}

// Called when the game starts or when spawned
void ANS_BaseItem::BeginPlay()
{
	Super::BeginPlay();

	if (!ItemsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item DataTable assigned on %s"), *GetName());
		return;
	}

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemType = ItemData->ItemType;
		WeaponType = ItemData->WeaponType;
		WeaponData = ItemData->WeaponData;
		ItemName = ItemData->ItemTextData.ItemName;
		NumericData = ItemData->ItemNumericData;
		GetItemSound = ItemData->ItemAssetData.UseSound;
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		Icon = ItemData->ItemAssetData.Icon;
		TextData = ItemData->ItemTextData;          
		AssetData = ItemData->ItemAssetData;
	}

	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Name = ItemName;
	InstanceInteractableData.Action = FText::FromString("to pick up");
}

void ANS_BaseItem::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
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

ANS_BaseItem* ANS_BaseItem::CreateItemCopy()
{
	//ANS_BaseItem* ItemCopy = NewObject<ANS_BaseItem>(this, GetClass());
	if (!GetWorld()) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner(); // 인벤토리의 Owner
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANS_BaseItem* ItemCopy = GetWorld()->SpawnActor<ANS_BaseItem>(GetClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!ItemCopy)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to Spawn ItemCopy in CreateItemCopy()"));
		return nullptr;
	}

	ItemCopy->ItemDataRowName = this->ItemDataRowName;
	ItemCopy->ItemName = this->ItemName;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->WeaponType = this->WeaponType;
	ItemCopy->WeaponData = this->WeaponData;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->ItemsDataTable = this->ItemsDataTable;
	ItemCopy->bisCopy = true;

	UE_LOG(LogTemp, Warning, TEXT("[CreateItemCopy] 복제된 아이템: %s, RowName: %s"),
		*ItemCopy->GetName(),
		*ItemCopy->ItemDataRowName.ToString());

	return ItemCopy;
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


