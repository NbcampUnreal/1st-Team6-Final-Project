#include "Item/NS_BaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"



ANS_BaseItem::ANS_BaseItem() : bisCopy(false), bisPickup(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	ItemStaticMesh->SetIsReplicated(true); // 복제 설정
	RootComponent = ItemStaticMesh;
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
	if (ItemStaticMesh && ItemData->ItemAssetData.StaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemData->ItemAssetData.StaticMesh);
	}
}

void ANS_BaseItem::ResetItemFlags()
{
	bisCopy = false;
	bisPickup = false;
}

void ANS_BaseItem::OnRep_ItemDataRowName()
{
	if (ItemDataRowName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT(" OnRep_ItemDataRowName - ItemDataRowName이 None 상태임: %s"), *GetName());
		return;
	}

	if (!ItemsDataTable)
	{
		if (const UNS_GameInstance* GI = Cast<UNS_GameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			ItemsDataTable = GI->ItemDataTable;
			UE_LOG(LogTemp, Warning, TEXT("클라에서 ItemsDataTable 성공적으로 할당: %s"), *GetName());
		}
	}

	const FNS_ItemDataStruct* ItemData = GetItemData();
	if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ OnRep_ItemDataRowName 내부: 데이터 테이블 또는 RowName 없음"));
        return;
    }

    ItemType = ItemData->ItemType;
    WeaponType = ItemData->WeaponType;
    WeaponData = ItemData->WeaponData;
    ItemName = ItemData->ItemTextData.ItemName;
    NumericData = ItemData->ItemNumericData;
    TextData = ItemData->ItemTextData;
    AssetData = ItemData->ItemAssetData;

    UE_LOG(LogTemp, Warning, TEXT("OnRep_ItemDataRowName 호출됨: %s | RowName: %s"), *GetName(), *ItemDataRowName.ToString());
}

void ANS_BaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_BaseItem, Quantity);
	DOREPLIFETIME(ANS_BaseItem, ItemDataRowName);
	DOREPLIFETIME(ANS_BaseItem, ItemType);
	DOREPLIFETIME(ANS_BaseItem, WeaponType);
	DOREPLIFETIME(ANS_BaseItem, ItemName);
	DOREPLIFETIME(ANS_BaseItem, NumericData);
	DOREPLIFETIME(ANS_BaseItem, AssetData);
	DOREPLIFETIME(ANS_BaseItem, TextData);
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

void ANS_BaseItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, NumericData.isStackable ? NumericData.MaxStack : 1);

		if (OwingInventory)
		{
			if (Quantity <= 0)
			{
				OwingInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
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

bool ANS_BaseItem::IsSupportedForNetworking() const
{
	return true;
}

