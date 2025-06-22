// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AnimNotify/NS_AN_EquipQuickSlotItem.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"

UNS_AN_EquipQuickSlotItem::UNS_AN_EquipQuickSlotItem()
{
}

void UNS_AN_EquipQuickSlotItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
    
    // 퀵슬롯 컴포넌트 확인
    UNS_QuickSlotComponent* QuickSlotComp = PlayerCharacter->GetQuickSlotComponent();
    if (!QuickSlotComp)
    {
        return;
    }
    
    // 무기 컴포넌트 확인
    UNS_EquipedWeaponComponent* WeaponComp = PlayerCharacter->FindComponentByClass<UNS_EquipedWeaponComponent>();
    if (!WeaponComp)
    {
        return;
    }
    
    // 현재 선택된 퀵슬롯 인덱스 가져오기
    int32 CurrentSlotIndex = PlayerCharacter->GetCurrentQuickSlotIndex();
    
    // 슬롯에 아이템이 있는지 확인
    UNS_InventoryBaseItem* Item = QuickSlotComp->GetItemInSlot(CurrentSlotIndex);
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        // 무기 해제 처리 - 서버/클라이언트 상태에 따라 적절한 함수 호출
        if (PlayerCharacter->HasAuthority())
        {
            // 서버에서는 직접 멀티캐스트 호출
            WeaponComp->Multicast_UnequipWeapon();
            UE_LOG(LogTemp, Warning, TEXT("NS_AN_EquipQuickSlotItem: 서버에서 무기 해제"));
        }
        else
        {
            // 클라이언트에서는 서버 RPC 호출
            WeaponComp->Server_UnequipWeapon();
            UE_LOG(LogTemp, Warning, TEXT("NS_AN_EquipQuickSlotItem: 클라이언트에서 무기 해제 요청"));
        }
        
        return;
    }
    
    // 아이템 데이터 확인
    const FNS_ItemDataStruct* ItemData = Item->GetItemData();
    if (!ItemData) 
    {
        return;
    }
    
    // 장비 아이템이 아니면 무기 해제
    if (ItemData->ItemType != EItemType::Equipment) 
    {
        // 무기 해제 처리
        if (PlayerCharacter->HasAuthority())
        {
            WeaponComp->Multicast_UnequipWeapon();
        }
        else
        {
            WeaponComp->Server_UnequipWeapon();
        }
        
        return;
    }
    
    // 현재 장착된 무기 확인
    UNS_InventoryBaseItem* CurrentWeapon = WeaponComp->GetCurrentWeaponItem();
    
    // 이미 같은 무기가 장착되어 있으면 무시
    if (CurrentWeapon && CurrentWeapon == Item)
    {
        return;
    }
    
    // 서버/클라이언트 상태에 따라 적절한 함수 호출
    if (PlayerCharacter->HasAuthority())
    {
        // 서버에서는 직접 멀티캐스트 호출
        WeaponComp->MulticastEquipWeapon(ItemData->WeaponActorClass, Item);
        UE_LOG(LogTemp, Warning, TEXT("NS_AN_EquipQuickSlotItem: 서버에서 무기 장착 - %s"), *Item->GetName());
    }
    else
    {
        // 클라이언트에서는 서버 RPC 호출
        WeaponComp->ServerEquipWeapon(ItemData->WeaponActorClass, Item);
        UE_LOG(LogTemp, Warning, TEXT("NS_AN_EquipQuickSlotItem: 클라이언트에서 무기 장착 요청 - %s"), *Item->GetName());
    }
}

FString UNS_AN_EquipQuickSlotItem::GetNotifyName_Implementation() const
{
    return TEXT("NS Equip QuickSlot Item");
}
