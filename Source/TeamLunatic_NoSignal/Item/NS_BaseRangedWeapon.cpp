#include "Item/NS_BaseRangedWeapon.h"
#include "NiagaraComponent.h"
#include "Item/NS_BaseMagazine.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Item/NS_BaseAmmo.h"

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

bool ANS_BaseRangedWeapon::AttachMagazine(ANS_BaseMagazine* NewMagazine)
{
	if (!NewMagazine)
	{
		UE_LOG(LogTemp, Warning, TEXT("착용하려는 탄창이 null입니다."));
		return false;
	}

	if (NewMagazine->GetAmmoType() != AmmoType)
	{
		UE_LOG(LogTemp, Warning, TEXT("탄창 타입이 무기와 일치하지 않습니다."));
		
		return false;
	}

	if (CurrentMagazine)
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 탄창이 장착되어 있습니다. 교체하려면 먼저 분리해야 합니다."));
		return false;
	}

	CurrentMagazine = NewMagazine;
	UGameplayStatics::PlaySoundAtLocation(this, AttachMagSound, GetActorLocation());

	UE_LOG(LogTemp, Log, TEXT("탄창이 성공적으로 장착되었습니다."));
	return true;
}

ANS_BaseMagazine* ANS_BaseRangedWeapon::DetachMagazine()
{
	if (!CurrentMagazine)
		return nullptr;

	ANS_BaseMagazine* Detached = CurrentMagazine;
	CurrentMagazine = nullptr;

	UGameplayStatics::PlaySoundAtLocation(this, DetachMagSound, GetActorLocation());

	return Detached;
}

bool ANS_BaseRangedWeapon::CanFire() const
{
	return CurrentMagazine && !CurrentMagazine->IsEmpty();
}

bool ANS_BaseRangedWeapon::Fire()
{
	if (!CanFire())
	{
		UE_LOG(LogTemp, Log, TEXT("현재 탄창이 없이 없거나, 총알이 없음."));
		UGameplayStatics::PlaySoundAtLocation(this, UnFireSound, GetActorLocation());
		return false;
	}
	
	CurrentMagazine->ConsumeOne();

	UE_LOG(LogTemp, Log, TEXT("현재 총알 수 : %d."), CurrentMagazine->GetCurrentAmmo());
	UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());

	TSubclassOf<ANS_BaseAmmo> AmmoClass = CurrentMagazine->GetAmmoClass();
	if (AmmoClass)
	{
		const ANS_BaseAmmo* DefaultAmmo = AmmoClass->GetDefaultObject<ANS_BaseAmmo>();
		int32 Damage = DefaultAmmo->GetDamage();
	}


	return true;
}

void ANS_BaseRangedWeapon::PerformHitScan(int32 Damage)
{
	
}

