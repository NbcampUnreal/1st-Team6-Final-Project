#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraComponent.h"
#include "Item/NS_BaseMagazine.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/NS_BaseAmmo.h"
#include "Character/NS_PlayerController.h"

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
}

void ANS_BaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		MaxAmmo = ItemData->WeaponData.MaxAmmo;
	}

	InstanceInteractableData.Quantity = 1;
	InteractableData = InstanceInteractableData;
}

void ANS_BaseRangedWeapon::Reload(int32 AmmoToAdd)
{
	int32 AmmoBefore = CurrentAmmo;
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoToAdd, 0, MaxAmmo);

	UE_LOG(LogTemp, Log, TEXT("[Reload] 장전됨: %d → %d"), AmmoBefore, CurrentAmmo);
}