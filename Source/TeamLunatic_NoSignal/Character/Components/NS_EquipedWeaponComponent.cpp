#include "NS_EquipedWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Item/NS_BaseMeleeWeapon.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Item/NS_BaseThrowWeapon.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/AnimInstance/NS_RangedWeaponAnimInstance.h"
#include "Components/StaticMeshComponent.h"  
#include "GameFlow/NS_GameInstance.h"

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

void UNS_EquipedWeaponComponent::SwapWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
{
    // 슬롯 → 무기 클래스 결정
    ServerEquipWeapon(WeaponClass, SourceItem);
}

void UNS_EquipedWeaponComponent::ServerEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
{
    MulticastEquipWeapon(WeaponClass, SourceItem);
}

void UNS_EquipedWeaponComponent::MulticastEquipWeapon_Implementation(TSubclassOf<ANS_BaseWeapon> WeaponClass, UNS_InventoryBaseItem* SourceItem)
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
    NewWpn->OwningInventoryItem = SourceItem;

    const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
    const FName SocketName = NewWpn->AttachSocketName;

    // MeleeWeapon 근거리 무기
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
    // RangeWeapon 원거리 무기
    else if (auto Ranged = Cast<ANS_BaseRangedWeapon>(NewWpn))
    {
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
    // ThrowWeapon 투척 병
    else if (auto Throw = Cast<ANS_BaseThrowWeapon>(NewWpn))
    {
        if (Throw->ItemStaticMesh)
        {
            Throw->ItemStaticMesh->AttachToComponent(
                OwnerCharacter->GetMesh(), Rules, SocketName);
        }

        if (Throw->ArmsMeshComp)
        {
            Throw->ArmsMeshComp->AttachToComponent(
                OwnerCharacter->FirstPersonArms, Rules, SocketName);
        }
    }

    // 현재 무기 설정
    CurrentWeapon = NewWpn;
    // 무기타입 갱신
    WeaponType = NewWpn->GetWeaponType();
}
void UNS_EquipedWeaponComponent::UnequipWeapon()
{
    Server_UnequipWeapon();
}
void UNS_EquipedWeaponComponent::Server_UnequipWeapon_Implementation()
{
    Multicast_UnequipWeapon();
}

void UNS_EquipedWeaponComponent::Multicast_UnequipWeapon_Implementation()
{
    if (!OwnerCharacter || !CurrentWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Unequip] 캐릭터 또는 무기 없음"));
        return;
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
    // 유효성 검사: 캐릭터나 무기 없으면 종료
    if (!OwnerCharacter || !CurrentWeapon)
        return;

    // 무기 타입 확인: 원거리 무기 또는 권총이 아니면 재장전 불가
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
            if (Item->ItemType == EItemType::Equipment && Item->WeaponType == EWeaponType::Ammo)
            {
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

