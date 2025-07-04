﻿#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Item/NS_BaseMeleeWeapon.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/AnimInstance/NS_RangedWeaponAnimInstance.h"
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
}

void UNS_EquipedWeaponComponent::SwapWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
{
    UE_LOG(LogTemp, Warning, TEXT("[SwapWeapon] 시작 - 무기 클래스: %s, 아이템: %s"), 
        *GetNameSafe(WeaponClass.Get()), *GetNameSafe(SourceItem));
    
    // 유효성 검사
    if (!WeaponClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[SwapWeapon] 무기 클래스가 없음"));
        return;
    }
    
    if (!SourceItem)
    {
        UE_LOG(LogTemp, Error, TEXT("[SwapWeapon] 소스 아이템이 없음"));
        return;
    }
    
    // 네트워크 모드에 따라 적절한 함수 호출
    if (GetOwnerRole() == ROLE_Authority)
    {
        // 서버에서 직접 호출
        MulticastEquipWeapon(WeaponClass, SourceItem);
        UE_LOG(LogTemp, Warning, TEXT("[SwapWeapon] 서버에서 직접 멀티캐스트 호출"));
    }
    else
    {
        // 클라이언트에서 서버에 요청
        ServerEquipWeapon(WeaponClass, SourceItem);
        UE_LOG(LogTemp, Warning, TEXT("[SwapWeapon] 클라이언트에서 서버 요청"));
    }
}

void UNS_EquipedWeaponComponent::ServerEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
{
    MulticastEquipWeapon(WeaponClass, SourceItem);
}

void UNS_EquipedWeaponComponent::MulticastEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
{
    if (CurrentWeapon && GetOwnerRole() == ROLE_Authority)
    {
        if (auto* OldRangedWeapon = Cast<ANS_BaseRangedWeapon>(CurrentWeapon))
        {
            UE_LOG(LogTemp, Warning, TEXT("[Equip] 기존 무기 탄약 저장: %d"), OldRangedWeapon->CurrentAmmo);
            OldRangedWeapon->UpdateAmmoToInventory();
        }
    }
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
                UE_LOG(LogTemp, Warning, TEXT("[MulticastEquipWeapon] 기존 무기 제거: %s"), *Attached->GetName());
            }
        }
    }
    
    if (!WeaponClass || !OwnerCharacter) 
    {
        UE_LOG(LogTemp, Error, TEXT("[MulticastEquipWeapon] 무기 클래스 또는 소유자 캐릭터가 없음"));
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner      = OwnerCharacter;
    Params.Instigator = OwnerCharacter;

    ANS_BaseWeapon* NewWpn = GetWorld()->SpawnActor<ANS_BaseWeapon>(
        WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (!NewWpn) 
    {
        UE_LOG(LogTemp, Error, TEXT("[MulticastEquipWeapon] 무기 생성 실패"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MulticastEquipWeapon] 새 무기 생성: %s"), *NewWpn->GetName());
    
    NewWpn->SetOwner(OwnerCharacter);
    NewWpn->OwningInventoryItem = SourceItem;

    const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
    const FName SocketName = NewWpn->AttachSocketName;

    // 무기 타입에 따라 적절한 처리
    if (auto Melee = Cast<ANS_BaseMeleeWeapon>(NewWpn))
    {
        UE_LOG(LogTemp, Warning, TEXT("[MulticastEquipWeapon] 근접 무기 장착"));
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
    // RangeWeapon 원거리 무기
    else if (auto Ranged = Cast<ANS_BaseRangedWeapon>(NewWpn))
    {
        UE_LOG(LogTemp, Warning, TEXT("[MulticastEquipWeapon] 원거리 무기 장착"));
        //원거리 무기 총알 타입 설정
        if (SourceItem)
        {
            Ranged->WeaponData = SourceItem->WeaponData;
            Ranged->CurrentAmmo = SourceItem->CurrentAmmo; 
            UE_LOG(LogTemp, Warning, TEXT("[Equip] 탄약 복사: %d → %d"), SourceItem->CurrentAmmo, Ranged->CurrentAmmo);
        }

        // 플레이어한테만 보이는 메쉬를 팔에 부착
        // RangedWeaponMeshComp가 유효한지 확인
        if (Ranged->RangedWeaponMeshComp)
        {
            Ranged->RangedWeaponMeshComp->AttachToComponent(
                OwnerCharacter->GetMesh(), Rules, SocketName);

            if (UNS_RangedWeaponAnimInstance* WeaponAnim = Cast<UNS_RangedWeaponAnimInstance>(
           Ranged->RangedWeaponMeshComp->GetAnimInstance()))
            {
                WeaponAnim->OwnerCharacter = OwnerCharacter;
            }
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
    
    UE_LOG(LogTemp, Warning, TEXT("[MulticastEquipWeapon] 무기 장착 완료: %s, 타입: %d"), 
        *NewWpn->GetName(), static_cast<int32>(WeaponType));
}
void UNS_EquipedWeaponComponent::UnequipWeapon()
{
    // 네트워크 모드에 따라 적절한 함수 호출
    if (GetOwnerRole() == ROLE_Authority)
    {
        // 서버에서 직접 멀티캐스트 호출
        Multicast_UnequipWeapon();
        UE_LOG(LogTemp, Warning, TEXT("[UnequipWeapon] 서버에서 직접 멀티캐스트 호출"));
    }
    else
    {
        // 클라이언트에서 서버에 요청
        Server_UnequipWeapon();
        UE_LOG(LogTemp, Warning, TEXT("[UnequipWeapon] 클라이언트에서 서버 요청"));
    }
}

void UNS_EquipedWeaponComponent::Server_UnequipWeapon_Implementation()
{
    // 서버에서 모든 클라이언트에 멀티캐스트
    Multicast_UnequipWeapon();
    UE_LOG(LogTemp, Warning, TEXT("[Server_UnequipWeapon] 서버에서 멀티캐스트 호출"));
}

void UNS_EquipedWeaponComponent::Multicast_UnequipWeapon_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Unequip] 캐릭터 또는 무기 없음"));
        return;
    }

    if (GetOwnerRole() == ROLE_Authority)
    {
        if (auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(CurrentWeapon))
        {
            UE_LOG(LogTemp, Warning, TEXT("[Unequip] 현재 무기 탄약: %d"), RangedWeapon->CurrentAmmo);

            RangedWeapon->UpdateAmmoToInventory();
        }
    }

    // 장착 중인 무기 파괴
    TArray<AActor*> AttachedWeapons;
    OwnerCharacter->GetAttachedActors(AttachedWeapons);

    for (AActor* Attached : AttachedWeapons)
    {
        if (Attached == CurrentWeapon)
        {
            Attached->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            Attached->Destroy();
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[Unequip] 무기 해제됨: %s"), *CurrentWeapon->GetName());

    // 상태 초기화
    CurrentWeapon = nullptr;
    WeaponType = EWeaponType::Unarmed;
    IsAttack = false;
    IsEmpty = false;
}

void UNS_EquipedWeaponComponent::Server_Reload_Implementation()
{
    Multicast_Reload();
}

void UNS_EquipedWeaponComponent::Multicast_Reload_Implementation()
{
    // 캐릭터나 무기 없으면 종료
    if (!OwnerCharacter || !CurrentWeapon)
        return;

    // 원거리 무기 또는 권총이 아니면 재장전 불가
    const EWeaponType CurrentType = CurrentWeapon->GetWeaponType();
    if (CurrentType != EWeaponType::Ranged && CurrentType != EWeaponType::Pistol)
        return;

    // 원거리 무기로 캐스팅 시도
    auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(CurrentWeapon);
    if (!RangedWeapon)
        return;

    // 이미 최대 탄약이면 재장전할 필요 없음
    if (RangedWeapon->CurrentAmmo >= RangedWeapon->MaxAmmo)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Reload] 이미 탄약이 가득 참"));
        return;
    }

    // 필요한 탄약량 계산
    const int32 NeededAmmo = RangedWeapon->MaxAmmo - RangedWeapon->CurrentAmmo;

    // 인벤토리에서 탄약 찾기
    if (auto* Inventory = OwnerCharacter->FindComponentByClass<UInventoryComponent>())
    {
        bool bReloaded = false;

        for (UNS_InventoryBaseItem* Item : Inventory->GetInventoryContents())
        {
            if (!Item || Item->GetQuantity() <= 0)
                continue;

            UE_LOG(LogTemp, Warning, TEXT("[Reload] 검사 중인 아이템: %p | Name: %s | 수량: %d"),
                Item, *Item->GetName(), Item->GetQuantity());

            // 탄약 아이템인지 확인
            if (Item->ItemType == EItemType::Equipment && Item->WeaponType == EWeaponType::Ammo &&
                Item->WeaponData.AmmoType == RangedWeapon->WeaponData.AmmoType)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Reload] 무기 AmmoType: %d, 아이템 AmmoType: %d"),
                    static_cast<int32>(RangedWeapon->WeaponData.AmmoType),
                    static_cast<int32>(Item->WeaponData.AmmoType));
                const int32 AmmoAvailable = Item->GetQuantity();
                const int32 AmmoToLoad = FMath::Min(NeededAmmo, AmmoAvailable);

                UE_LOG(LogTemp, Warning, TEXT("[Reload] 아이템: %p | OwingInventory: %s"),
                    Item, *GetNameSafe(Item->OwingInventory));

                if (AmmoToLoad > 0)
                {
                    RangedWeapon->Reload(AmmoToLoad);

                    if (Item->OwingInventory)
                    {
                        const int32 RemovedAmmo = Item->OwingInventory->RemoveAmountOfItem(Item, AmmoToLoad);
                        UE_LOG(LogTemp, Warning, TEXT("[Reload] %d발 장전 완료. 남은 탄약: %d"), AmmoToLoad, Item->GetQuantity());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("[Reload] OwingInventory가 null입니다! Remove 실패"));
                    }

                    bReloaded = true;
                    break;
                }
            }
        }
        if (!bReloaded)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Reload] 사용할 수 있는 탄약 없음 또는 탄약 수량 부족"));
        }
    }
}

UNS_InventoryBaseItem* UNS_EquipedWeaponComponent::GetCurrentWeaponItem() const
{
    return CurrentWeapon ? CurrentWeapon->OwningInventoryItem : nullptr;
}

