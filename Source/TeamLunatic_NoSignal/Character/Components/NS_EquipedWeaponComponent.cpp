#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseWeapon.h" 
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"

UNS_EquipedWeaponComponent::UNS_EquipedWeaponComponent()
{
    SetIsReplicatedByDefault(true);
}

void UNS_EquipedWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ANS_PlayerCharacterBase>(GetOwner());
}

void UNS_EquipedWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UNS_EquipedWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsAttack);
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsReload);
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsEmpty);
}

void UNS_EquipedWeaponComponent::SwapWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass)
{
    // 슬롯 → 무기 클래스 결정
    ServerEquipWeapon(WeaponClass);
}

void UNS_EquipedWeaponComponent::ServerEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass)
{
    MulticastEquipWeapon(WeaponClass);
}


void UNS_EquipedWeaponComponent::MulticastEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass)
{
    // 기존에 무기가 장착되어있으면 제거
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }

    if (WeaponClass && OwnerCharacter)
    {
        FActorSpawnParameters Params;
        Params.Owner = GetOwner();
        Params.Instigator = OwnerCharacter;
        
        ANS_BaseWeapon* NewWpn = GetWorld()->SpawnActor<ANS_BaseWeapon>(
            WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

        if (NewWpn)
        {
            NewWpn->AttachToComponent(
                OwnerCharacter->GetMesh(),
                FAttachmentTransformRules::SnapToTargetIncludingScale,
                WeaponClass->GetDefaultObject<ANS_BaseWeapon>()->AttachSocketName
            );

            CurrentWeapon = NewWpn;
        }
    }
}

void UNS_EquipedWeaponComponent::StartAttack()
{
    // 비무장 : 공격없음
    if (!CurrentWeapon)
    {
        IsAttack = false;
        return;
    }

    // 무기 변경중이면 공격 불가
	if (OwnerCharacter && OwnerCharacter->IsChangingWeapon)
    {
        IsAttack = false;
        return;
    }

	// 무기 타입에 따라 공격 처리
    if (CurrentWeapon->GetWeaponType() == EWeaponType::Ranged) // 원거리
    {
        // 현재 탄창 비어있음
		if (IsEmpty)
        {
            IsAttack = false;
            return;
        }

		if (IsReload)
		{
			IsAttack = false;
			return;
		}

		IsAttack = true;

    }
    else if (CurrentWeapon->GetWeaponType() == EWeaponType::Melee)
    {
        IsAttack = true;
    }


}

void UNS_EquipedWeaponComponent::StopAttack()
{	
	IsAttack = false;
}


void UNS_EquipedWeaponComponent::Reload()
{
	// 원거리 무장 아님 : 재장전 무조건 false
	//if (CurrentWeapon && CurrentWeapon->GetWeaponType() != EWeaponType::Ranged)
	//{
	//	IsReload = false;
	//	return;
	//}

    //TODO 인벤토리 살펴보기
    //인벤토리내 무기에 맞는 탄창or탄약이 있는지 체크
    //if(탄창으로 구현) : 현재 탄창을 인벤토리에 넣고(부족하면)

    IsReload = true;
}

