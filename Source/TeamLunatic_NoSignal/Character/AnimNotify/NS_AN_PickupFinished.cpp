// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AnimNotify/NS_AN_PickupFinished.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"

UNS_AN_PickupFinished::UNS_AN_PickupFinished()
{
}

void UNS_AN_PickupFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    // 메쉬 컴포넌트 또는 소유자 확인
    if (!MeshComp || !MeshComp->GetOwner())
    {
        return;
    }
    
    // 애니메이션 에디터 미리보기 액터인 경우 무시
    FString OwnerName = MeshComp->GetOwner()->GetName();
    if (OwnerName.Contains(TEXT("AnimationEditorPreviewActor")) || 
        OwnerName.Contains(TEXT("PreviewMesh")))
    {
        return;
    }
    
    // 메쉬 컴포넌트의 소유자를 플레이어 캐릭터로 캐스팅
    ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(MeshComp->GetOwner());
    if (!PlayerCharacter)
    {
        return;
    }
    
    // IsPickUp 변수를 false로 설정
    PlayerCharacter->IsPickUp = false;
    UE_LOG(LogTemp, Warning, TEXT("PickupFinished: 아이템 획득 애니메이션 완료 (IsPickUp = false)"));
    
    // 퀵슬롯 컴포넌트 확인
    UNS_QuickSlotComponent* QuickSlotComp = PlayerCharacter->GetQuickSlotComponent();
    if (!QuickSlotComp)
    {
        return;
    }
    
    // 현재 선택된 퀵슬롯 인덱스의 아이템 확인
    int32 CurrentSlotIndex = PlayerCharacter->GetCurrentQuickSlotIndex();
    UNS_InventoryBaseItem* Item = QuickSlotComp->GetItemInSlot(CurrentSlotIndex);
    
    // 현재 선택된 슬롯에 장비 아이템이 있고 탄약이 아닌 경우 장착 애니메이션 실행
    if (Item && Item->ItemType == EItemType::Equipment && Item->WeaponType != EWeaponType::Ammo)
    {
        // 무기 컴포넌트 확인
        UNS_EquipedWeaponComponent* WeaponComp = PlayerCharacter->FindComponentByClass<UNS_EquipedWeaponComponent>();
        if (WeaponComp)
        {
            // 현재 장착된 무기 확인
            UNS_InventoryBaseItem* CurrentWeapon = WeaponComp->GetCurrentWeaponItem();
            
            // 이미 같은 무기가 장착되어 있으면 무시
            if (CurrentWeapon && CurrentWeapon == Item)
            {
                UE_LOG(LogTemp, Warning, TEXT("PickupFinished: 이미 같은 무기가 장착되어 있음 - 무기 교체 애니메이션 무시"));
                return;
            }
            
            // 현재 선택된 슬롯의 아이템으로 무기 교체 애니메이션 실행
            PlayerCharacter->IsChangeAnim = true;
            UE_LOG(LogTemp, Warning, TEXT("PickupFinished: 무기 장착 애니메이션 시작 (IsChangeAnim = true), 슬롯: %d, 아이템: %s"), 
                CurrentSlotIndex + 1, *Item->GetName());
            
            // 1.4초 후 애니메이션 플래그 리셋 타이머 설정
            FTimerHandle ResetAnimTimerHandle;
            PlayerCharacter->GetWorldTimerManager().SetTimer(
                ResetAnimTimerHandle,
                FTimerDelegate::CreateLambda([PlayerCharacter]() { 
                    PlayerCharacter->IsChangeAnim = false;
                    UE_LOG(LogTemp, Warning, TEXT("PickupFinished: 무기 교체 애니메이션 플래그 리셋 (IsChangeAnim = false)"));
                }),
                1.4f,
                false
            );
        }
    }
}

FString UNS_AN_PickupFinished::GetNotifyName_Implementation() const
{
    return TEXT("NS Pickup Finished");
}
