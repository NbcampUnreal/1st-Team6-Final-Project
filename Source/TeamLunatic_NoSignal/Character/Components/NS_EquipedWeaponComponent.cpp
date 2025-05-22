#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "WeaponBase.h"  // AWeaponBase 헤더
#include "GameFramework/Character.h"
#include "Engine/World.h"

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

void UNS_EquipedWeaponComponent::SwapWeapon(int32 SlotIndex)
{
    if (!GetOwner()->HasAuthority())
        return;

    // 슬롯 → 무기 클래스 결정
    TSubclassOf<AWeaponBase> NewClass = GetWeaponClassFromSlot(SlotIndex);
    ServerEquipWeapon(NewClass);
}

void UNS_EquipedWeaponComponent::ServerEquipWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
    // 기존 장착 해제
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }

    // 새 무기 스폰 & 부착
    if (WeaponClass && OwnerCharacter)
    {
        FActorSpawnParameters Params;
        Params.Owner = GetOwner();
        Params.Instigator = OwnerCharacter;
        AWeaponBase* NewWpn = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (NewWpn)
        {
            NewWpn->AttachToComponent(OwnerCharacter->GetMesh(),
                                      FAttachmentTransformRules::SnapToTargetIncludingScale,
                                      WeaponAttachSocketName);
            CurrentWeapon = NewWpn;
        }
    }

    // 클라이언트에도 동일하게 복제
    MulticastEquipWeapon(WeaponClass);
}

void UNS_EquipedWeaponComponent::MulticastEquipWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
    // 서버 쪽은 이미 처리했으므로 클라이언트만 동작
    if (GetOwner()->HasAuthority())
        return;

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
        AWeaponBase* NewWpn = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (NewWpn)
        {
            NewWpn->AttachToComponent(OwnerCharacter->GetMesh(),
                                      FAttachmentTransformRules::SnapToTargetIncludingScale,
                                      WeaponAttachSocketName);
            CurrentWeapon = NewWpn;
        }
    }
}

void UNS_EquipedWeaponComponent::Fire()
{
    if (CurrentWeapon && OwnerCharacter->HasAuthority())
    {
        CurrentWeapon->StartFire();
    }
}

void UNS_EquipedWeaponComponent::Reload()
{
    if (CurrentWeapon && OwnerCharacter->HasAuthority())
    {
        CurrentWeapon->StartReload();
    }
}

TSubclassOf<AWeaponBase> UNS_EquipedWeaponComponent::GetWeaponClassFromSlot(int32 SlotIndex) const
{
    // TODO: 나중에 InventoryComponent 에서 꺼내는 로직으로 대체
    // 예시) return OwnerCharacter->GetInventory()->GetWeaponClassAtSlot(SlotIndex);
    return nullptr;
}
