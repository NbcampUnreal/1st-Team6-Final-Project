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

void ANS_BaseRangedWeapon::Fire()
{
	if (!CanFire())
	{
		UE_LOG(LogTemp, Log, TEXT("현재 탄창이 없이 없거나, 총알이 없음."));
		UGameplayStatics::PlaySoundAtLocation(this, UnFireSound, GetActorLocation());
		return;
	}
	
	CurrentMagazine->ConsumeOne();

	UE_LOG(LogTemp, Log, TEXT("현재 총알 수 : %d."), CurrentMagazine->GetCurrentAmmo());
	UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());

	TSubclassOf<ANS_BaseAmmo> AmmoClass = CurrentMagazine->GetAmmoClass();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANS_BaseAmmo* TempAmmo = GetWorld()->SpawnActor<ANS_BaseAmmo>(
		AmmoClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!TempAmmo)
	{
		UE_LOG(LogTemp, Error, TEXT("탄환 생성 실패"));
		return;
	}

	int32 AmmoDamage = TempAmmo->GetDamage(); // BaseAmmo 내부에서 초기화된 Damage
	TempAmmo->Destroy();

	UE_LOG(LogTemp, Log, TEXT("발사! Damage: %d"), AmmoDamage);

	// 히트스캔 처리
	PerformHitScan(AmmoDamage);
}

void ANS_BaseRangedWeapon::PerformHitScan(int32 damage)
{

}

//탄환 확인