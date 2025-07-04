#include "Item/NS_BaseMagazine.h"
#include "Components/StaticMeshComponent.h"
#include "Item/NS_InventoryBaseItem.h"

ANS_BaseMagazine::ANS_BaseMagazine()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	RootComponent = ItemStaticMesh;

	CurrentAmmo = 0;
}

void ANS_BaseMagazine::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		ItemMesh = ItemData->ItemAssetData.StaticMesh;
		MaxAmmo = ItemData->WeaponData.MaxAmmo;
	}

	//Setting
	ItemStaticMesh->SetStaticMesh(ItemMesh);
	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;
}

bool ANS_BaseMagazine::IsEmpty() const
{
	return CurrentAmmo <= 0;
}

bool ANS_BaseMagazine::isFull() const
{
	return CurrentAmmo >= MaxAmmo;
}

bool ANS_BaseMagazine::ConsumeOne()
{
	if (CurrentAmmo <= 0)
	{
		return false;
	}

	--CurrentAmmo;
	return true;
}

int32 ANS_BaseMagazine::ConsumeAll()
{
	int32 Taken = CurrentAmmo;
	CurrentAmmo = 0;
	return Taken;
}

int32 ANS_BaseMagazine::RemoveAmmo(int32 Amount)
{
	int32 ActualRemoved = FMath::Min(CurrentAmmo, Amount);
	CurrentAmmo -= ActualRemoved;
	return ActualRemoved;
}

bool ANS_BaseMagazine::Refill(int32 Amount, EAmmoType InputAmmoType)
{
	if (InputAmmoType != AmmoType)
	{
		UE_LOG(LogTemp, Warning, TEXT("탄환 타입이 맞지 않음: %s != %s"),
			*UEnum::GetValueAsString(InputAmmoType),
			*UEnum::GetValueAsString(AmmoType));
		return false;
	}

	int32 Capacity = MaxAmmo - CurrentAmmo;
	int32 AmmoToAdd = FMath::Min(Amount, Capacity);

	CurrentAmmo += AmmoToAdd;

	UE_LOG(LogTemp, Log, TEXT("탄창에 %d발을 채움 (현재: %d / %d)"),
		AmmoToAdd, CurrentAmmo, MaxAmmo);

	return AmmoToAdd > 0;
}

bool ANS_BaseMagazine::CanUseAmmoType(EAmmoType InputType) const
{
	return InputType == AmmoType;
}

int32 ANS_BaseMagazine::GetRemainingCapacity() const
{
	return MaxAmmo - CurrentAmmo;
}

void ANS_BaseMagazine::InitializeFromItem(UNS_InventoryBaseItem* Item)
{
	if (!Item) return;

	// 데이터 연동
	const FNS_ItemDataStruct* ItemData = Item->GetItemData();
	if (!ItemData) return;

	// 탄창 관련 데이터 초기화
	ItemMesh = ItemData->ItemAssetData.StaticMesh;
	MaxAmmo = ItemData->WeaponData.MaxAmmo;
	AmmoType = ItemData->WeaponData.AmmoType;

	// 처음에는 전부 채워져있다고 가정
	CurrentAmmo = MaxAmmo;

	// 메쉬 반영
	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemMesh);
	}

	// 인터랙션 정보 갱신
	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;

	UE_LOG(LogTemp, Log, TEXT("InitializeFromItem: 탄창 초기화 완료 (%s, MaxAmmo: %d, AmmoType: %s)"),
		*GetNameSafe(this), MaxAmmo, *UEnum::GetValueAsString(AmmoType));
}






