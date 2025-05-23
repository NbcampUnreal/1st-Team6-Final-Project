#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseWeapon.h" 
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Item/NS_BaseMeleeWeapon.h"

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
}

void UNS_EquipedWeaponComponent::SwapWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass)
{
    // if (!GetOwner()->HasAuthority())
    //     return;

    // 슬롯 → 무기 클래스 결정
    ServerEquipWeapon(WeaponClass);
}

void UNS_EquipedWeaponComponent::ServerEquipWeapon_Implementation(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass)
{
    // if (CurrentWeapon)
    // {
    //     CurrentWeapon->Destroy();
    //     CurrentWeapon = nullptr;
    // }
    //
    // if (WeaponClass && OwnerCharacter)
    // {
    //     FActorSpawnParameters Params;
    //     Params.Owner = GetOwner();
    //     Params.Instigator = OwnerCharacter;
    //     
    //     ANS_BaseMeleeWeapon* NewWpn = GetWorld()->SpawnActor<ANS_BaseMeleeWeapon>(
    //         WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    //
    //     if (NewWpn)
    //     {
    //         NewWpn->AttachToComponent(
    //             OwnerCharacter->GetMesh(),
    //             FAttachmentTransformRules::SnapToTargetIncludingScale,
				// WeaponClass->GetDefaultObject<ANS_BaseMeleeWeapon>()->WeaponSocketName
    //         );
    //
    //         CurrentWeapon = NewWpn;
    //     }
    // }

    MulticastEquipWeapon(WeaponClass);
}


void UNS_EquipedWeaponComponent::MulticastEquipWeapon_Implementation(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass)
{
    // 서버 쪽은 이미 처리했으므로 클라이언트만 동작
    // if (GetOwner()->HasAuthority())
    //     return;

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
        
        ANS_BaseMeleeWeapon* NewWpn = GetWorld()->SpawnActor<ANS_BaseMeleeWeapon>(
            WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

        if (NewWpn)
        {
            NewWpn->AttachToComponent(
                OwnerCharacter->GetMesh(),
                FAttachmentTransformRules::SnapToTargetIncludingScale,
                WeaponClass->GetDefaultObject<ANS_BaseMeleeWeapon>()->WeaponSocketName
            );

            CurrentWeapon = NewWpn;
        }
    }
}

void UNS_EquipedWeaponComponent::Fire()
{
    // if (CurrentWeapon && OwnerCharacter->HasAuthority())
    // {
    //     CurrentWeapon->StartFire();
    // }
}

void UNS_EquipedWeaponComponent::Reload()
{
    // if (CurrentWeapon && OwnerCharacter->HasAuthority())
    // {
    //     CurrentWeapon->StartReload();
    // }
}

