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
	
	InstanceInteractableData.Quantity = 1;

	InteractableData = InstanceInteractableData;
}

bool ANS_BaseRangedWeapon::CanFire() const
{
	return CurrentAmmo > 0;
}

void ANS_BaseRangedWeapon::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire() 호출됨"));

	if (!CanFire())
	{
		UE_LOG(LogTemp, Warning, TEXT("탄약 없음."));
		UGameplayStatics::PlaySoundAtLocation(this, UnFireSound, GetActorLocation());
		return;
	}

	--CurrentAmmo;

	UE_LOG(LogTemp, Log, TEXT("발사! 남은 탄약: %d / %d"), CurrentAmmo, MaxAmmo);
	UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());

	// 탄환 생성 생략

	// 충돌 판정 및 데미지 적용
	PerformHitScan(WeaponData.Damage);
}

void ANS_BaseRangedWeapon::Reload(int32 AmmoToAdd)
{
	int32 AmmoBefore = CurrentAmmo;
	CurrentAmmo = FMath::Clamp(CurrentAmmo + AmmoToAdd, 0, MaxAmmo);

	UE_LOG(LogTemp, Log, TEXT("[Reload] 장전됨: %d → %d"), AmmoBefore, CurrentAmmo);
}

void ANS_BaseRangedWeapon::PerformHitScan(int32 InDamage)
{
	// 히트스캔 로직 구현 예정
}