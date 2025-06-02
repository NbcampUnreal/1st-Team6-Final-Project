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
	DOREPLIFETIME(UNS_EquipedWeaponComponent, IsReload); // 장전중인지 확인 변수
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

void UNS_EquipedWeaponComponent::StartAttack()
{
    // // 비무장 : 공격없음
    // if (!CurrentWeapon)
    // {
    //     IsAttack = false;
    //     return;
    // }

 //    // 무기 변경중이면 공격 불가
	// if (OwnerCharacter && OwnerCharacter->IsChangingWeapon)
 //    {
 //        IsAttack = false;
 //        return;
 //    }
 //
	// // 무기 타입에 따라 공격 처리
 //    if (CurrentWeapon->GetWeaponType() == EWeaponType::Ranged // 원거리
 //        && CurrentWeapon->GetWeaponType() == EWeaponType::Pistol)
 //    {
 //        // 현재 탄창 비어있으면 return
	// 	if (IsEmpty)
 //        {
 //            IsAttack = false;
 //            return;
 //        }
 //        // 재장전 중이면 return
	// 	if (IsReload)
	// 	{
	// 		IsAttack = false;
	// 		return;
	// 	}
 //
	// 	IsAttack = true;
 //    }
 //    else if (CurrentWeapon->GetWeaponType() == EWeaponType::Melee)
 //    {
 //        IsAttack = true;
 //    }

    IsAttack = true;
}

void UNS_EquipedWeaponComponent::StopAttack()
{	
	IsAttack = false;
}


void UNS_EquipedWeaponComponent::Reload()
{
    // 현재 무기가 없거나, 원거리 무기가 아니면 재장전 불가
    if (!CurrentWeapon || CurrentWeapon->GetWeaponType() != EWeaponType::Ranged)
    {
        IsReload = false;
        return;
    }

    // 현재 무기를 원거리 무기로 캐스팅
    auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(CurrentWeapon);
    if (!RangedWeapon)
    {
        IsReload = false;
        return;
    }

    // 이미 탄약이 최대치면 재장전할 필요 없음
    if (RangedWeapon->CurrentAmmo >= RangedWeapon->MaxAmmo)
    {
        UE_LOG(LogTemp, Log, TEXT("현재 탄약이 이미 최대입니다."));
        IsReload = false;
        return;
    }

    // 실제 탄약 수를 인벤토리에서 가져오는 로직은 나중에 추가 예정
    // 현재는 단순히 탄약을 최대치로 채워줌
    RangedWeapon->CurrentAmmo = RangedWeapon->MaxAmmo;

    // 재장전 상태를 true로 설정 (애니메이션 등에 활용 가능)
    IsReload = true;

    // 로그 출력: 재장전된 탄약 수 표시
    UE_LOG(LogTemp, Log, TEXT("재장전 완료: %d / %d"), RangedWeapon->CurrentAmmo, RangedWeapon->MaxAmmo);
}

