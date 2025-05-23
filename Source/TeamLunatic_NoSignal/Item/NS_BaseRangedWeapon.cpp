#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"


ANS_BaseRangedWeapon::ANS_BaseRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	RangedWeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = RangedWeaponMeshComp;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	NiagaraComponent->SetupAttachment(RootComponent);
}

void ANS_BaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();

	if (ItemData)
	{
		RangedWeaponMesh = ItemData->ItemAssetData.SkeletalMesh;
	}

	RangedWeaponMeshComp->SetSkeletalMesh(RangedWeaponMesh);

	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}

