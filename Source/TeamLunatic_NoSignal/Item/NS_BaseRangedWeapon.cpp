#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraComponent.h"
#include "Item/NS_BaseMagazine.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/NS_BaseAmmo.h"
#include "Character/NS_PlayerController.h"
#include "Item/NS_InventoryBaseItem.h"


ANS_BaseRangedWeapon::ANS_BaseRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RangedWeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("I Can'tSee Mesh");
	RangedWeaponMeshComp->SetOwnerNoSee(true);
	RangedWeaponMeshComp->SetOnlyOwnerSee(false);

	ArmsMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("I See Mesh");
	ArmsMeshComp->SetOwnerNoSee(false);
	ArmsMeshComp->SetOnlyOwnerSee(true);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));

	bReplicates = true;
}

void ANS_BaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		MaxAmmo = ItemData->WeaponData.MaxAmmo;
		AmmoType = ItemData->WeaponData.AmmoType;
		CurrentAmmo = ItemData->CurrentAmmo;
	}

	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;
}

void ANS_BaseRangedWeapon::Reload(int32 AmmoToAdd)
{
	int32 AmmoBefore = CurrentAmmo;
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoToAdd, 0, MaxAmmo);

	if (OwningInventoryItem)
	{
		OwningInventoryItem->CurrentAmmo = CurrentAmmo;
	}

	UE_LOG(LogTemp, Log, TEXT("[Reload] 장전됨: %d → %d"), AmmoBefore, CurrentAmmo);
}

void ANS_BaseRangedWeapon::PrepareForDrop()
{
	CachedAmmoForDrop = CurrentAmmo;
	UE_LOG(LogTemp, Warning, TEXT("[PrepareForDrop] 무기 내에 탄약 저장: %d"), CachedAmmoForDrop);
}

void ANS_BaseRangedWeapon::UpdateAmmoToInventory()
{
	if (OwningInventoryItem)
	{
		// CachedAmmoForDrop 값이 최신이면 이것으로 저장
		OwningInventoryItem->CurrentAmmo = CachedAmmoForDrop > 0 ? CachedAmmoForDrop : CurrentAmmo;
		UE_LOG(LogTemp, Warning, TEXT("[UpdateAmmoToInventory] 서버에서 탄약 저장: %d"), OwningInventoryItem->CurrentAmmo);
	}
}