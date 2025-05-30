#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraComponent.h"

ANS_BaseRangedWeapon::ANS_BaseRangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RangedWeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("I Can'tSee Mesh");
	RangedWeaponMeshComp->SetOwnerNoSee(true);
	RangedWeaponMeshComp->SetOnlyOwnerSee(false);

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("I See Mesh");
	ArmsMesh->SetOwnerNoSee(false);
	ArmsMesh->SetOnlyOwnerSee(true);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
}

void ANS_BaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	const FNS_ItemDataStruct* ItemData = GetItemData();
	
	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}

