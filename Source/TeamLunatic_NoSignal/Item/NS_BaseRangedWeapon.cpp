#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"


ANS_BaseRangedWeapon::ANS_BaseRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	RangedWeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HideWeaponMesh"));
	RootComponent = RangedWeaponMeshComp;
	RangedWeaponMeshComp->SetOnlyOwnerSee(false);
	RangedWeaponMeshComp->CastShadow = true;
	RangedWeaponMeshComp->bCastDynamicShadow = true;

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = ArmsMesh;
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->CastShadow = false;
	ArmsMesh->bCastDynamicShadow = false;

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

