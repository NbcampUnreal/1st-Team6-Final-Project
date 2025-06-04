#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_BaseMeleeWeapon.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"   

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
    DOREPLIFETIME(UNS_EquipedWeaponComponent, CurrentWeapon); // 현재 무기 변수
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsAttack); // 공격중인지 확인 변수
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsEmpty); // 총알이 있는지 없는지 확인 변수
    DOREPLIFETIME(UNS_EquipedWeaponComponent, WeaponType); // 무기 타입 변수
    DOREPLIFETIME(UNS_EquipedWeaponComponent, CurrentWeapon); // 현재 장착중인 무기 변수

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
    // 기존 무기 제거
    if (OwnerCharacter)
    {
        TArray<AActor*> AttachedWeapons;
        OwnerCharacter->GetAttachedActors(AttachedWeapons);

        for (AActor* Attached : AttachedWeapons)
        {
            // 무기종류만
            if (Attached->IsA<ANS_BaseWeapon>())
            {
                // 부모 컴포넌트에서 분리 (안정성 확보)
                Attached->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                // 월드에서 완전 제거
                Attached->Destroy();
            }
        }
    }
    
    if (!WeaponClass || !OwnerCharacter) return;

    FActorSpawnParameters Params;
    Params.Owner      = OwnerCharacter;
    Params.Instigator = OwnerCharacter;

    ANS_BaseWeapon* NewWpn = GetWorld()->SpawnActor<ANS_BaseWeapon>(
        WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (!NewWpn) return;

    NewWpn->SetOwner(OwnerCharacter);
    
    const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
    const FName SocketName = NewWpn->AttachSocketName;

    // MeleeWeapon (근접 무기)
    if (auto Melee = Cast<ANS_BaseMeleeWeapon>(NewWpn))
    {
        // 플레이어한테만 보이는 메쉬를 팔에 부착
        // ArmsMesh가 유효한지 확인
        if (Melee->ArmsMeshComp) 
        {
            Melee->ArmsMeshComp->AttachToComponent(
                OwnerCharacter->FirstPersonArms, Rules, SocketName);
        }
        
        // 다른 플레이어에게 보이게 메쉬를 몸에 부착
        // ItemStaticMesh가 유효한지 확인
        if (Melee->ItemStaticMesh)
        {
            Melee->ItemStaticMesh->AttachToComponent(
                OwnerCharacter->GetMesh(), Rules, SocketName);
        }
    }
    // RangeWeapon (원거리 무기)
    else if (auto Ranged = Cast<ANS_BaseRangedWeapon>(NewWpn))
    {
        // 플레이어한테만 보이는 메쉬를 팔에 부착
        // RangedWeaponMeshComp가 유효한지 확인
        if (Ranged->RangedWeaponMeshComp)
        {
            Ranged->RangedWeaponMeshComp->AttachToComponent(
                OwnerCharacter->GetMesh(), Rules, SocketName);
        }
        
        // 다른 플레이어에게 보이게 메쉬를 몸에 부착
        // ArmsMesh가 유효한지 확인
        if (Ranged->ArmsMeshComp)
        {
            Ranged->ArmsMeshComp->AttachToComponent(
                OwnerCharacter->FirstPersonArms, Rules, SocketName);
        }
    }

    // 현재 무기 설정
    CurrentWeapon = NewWpn;
    // 무기타입 갱신
    WeaponType = NewWpn->GetWeaponType();
}

void UNS_EquipedWeaponComponent::Server_Reload_Implementation()
{
    Multicast_Reload();
}

void UNS_EquipedWeaponComponent::Multicast_Reload_Implementation()
{
    // 현재 무기가 없거나, 원거리 무기가 아니면 재장전 불가
    if (!CurrentWeapon ||
        CurrentWeapon->GetWeaponType() != EWeaponType::Ranged ||
        CurrentWeapon->GetWeaponType() != EWeaponType::Pistol )
    {
        return;
    }

    // 현재 무기를 원거리 무기로 캐스팅
    auto* CurrentWeaponBullet = Cast<ANS_BaseRangedWeapon>(CurrentWeapon);

    // 이미 탄약이 최대치면 재장전할 필요 없음
    if (CurrentWeaponBullet->CurrentAmmo >= CurrentWeaponBullet->MaxAmmo)
    {
        UE_LOG(LogTemp, Log, TEXT("현재 탄약이 이미 최대입니다."));
        return;
    }

    // 실제 탄약 수를 인벤토리에서 가져오는 로직은 나중에 추가 예정
    // 현재는 단순히 탄약을 최대치로 채워줌
    CurrentWeaponBullet->CurrentAmmo = CurrentWeaponBullet->MaxAmmo;
}

