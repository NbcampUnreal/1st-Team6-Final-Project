// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_CurrentWeaponPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "Item/NS_BaseWeapon.h"

void UNS_CurrentWeaponPanel::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 플레이어 캐릭터 참조 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
        if (PlayerCharacter)
        {
            EquipedWeaponComp = PlayerCharacter->GetEquipedWeaponComponent();
        }
    }
    
    // 초기에 텍스트 숨기기
    if (CurrentAndMaxAmmoText) CurrentAndMaxAmmoText->SetVisibility(ESlateVisibility::Collapsed);
    if (FireModeText) FireModeText->SetVisibility(ESlateVisibility::Collapsed);
    if (ItemQuantityText) ItemQuantityText->SetVisibility(ESlateVisibility::Collapsed);
}

void UNS_CurrentWeaponPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // 플레이어 캐릭터가 없는 경우 다시 참조 가져오기 시도
    if (!PlayerCharacter || !EquipedWeaponComp)
    {
        APlayerController* PC = GetOwningPlayer();
        if (PC)
        {
            PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PC->GetPawn());
            if (PlayerCharacter)
            {
                EquipedWeaponComp = PlayerCharacter->GetEquipedWeaponComponent();
            }
        }
    }
    
    // 무기 정보 업데이트 - 무기가 변경되었을 때만 업데이트
    if (EquipedWeaponComp)
    {
        UNS_InventoryBaseItem* WeaponItem = EquipedWeaponComp->GetCurrentWeaponItem();
        FName NewWeaponID = WeaponItem ? WeaponItem->ItemDataRowName : NAME_None;
        
        // 무기가 변경되었거나 초기 설정이 필요한 경우에만 업데이트
        if (NewWeaponID != CurrentWeaponID)
        {
            CurrentWeaponID = NewWeaponID;
            UpdateWeaponInfo(EquipedWeaponComp);
        }
        
        // 탄약 수는 매 프레임 업데이트 (사격 시 변경되기 때문)
        if (WeaponItem && (EquipedWeaponComp->WeaponType == EWeaponType::Pistol || EquipedWeaponComp->WeaponType == EWeaponType::Rifle))
        {
            if (CurrentAndMaxAmmoText)
            {
                FString AmmoText = FString::Printf(TEXT("%d / %d"), WeaponItem->CurrentAmmo, WeaponItem->WeaponData.MaxAmmo);
                CurrentAndMaxAmmoText->SetText(FText::FromString(AmmoText));
            }
        }
        else if (WeaponItem && EquipedWeaponComp->WeaponType == EWeaponType::ThrowActor)
        {
            if (ItemQuantityText)
            {
                ItemQuantityText->SetText(FText::AsNumber(WeaponItem->GetQuantity()));
            }
        }
    }
}

void UNS_CurrentWeaponPanel::UpdateWeaponInfo(UNS_EquipedWeaponComponent* InEquipedWeaponComp)
{
    if (!InEquipedWeaponComp) return;
    
    // 현재 장착된 무기 정보 가져오기
    ANS_BaseWeapon* CurrentWeapon = InEquipedWeaponComp->CurrentWeapon;
    UNS_InventoryBaseItem* WeaponItem = InEquipedWeaponComp->GetCurrentWeaponItem();
    EWeaponType WeaponType = InEquipedWeaponComp->WeaponType;
    
    // 무기 타입에 따라 UI 설정
    SetupUIByWeaponType(WeaponType, WeaponItem);
    
    // 원거리 무기인 경우 추가 정보 설정
    if ((WeaponType == EWeaponType::Pistol || WeaponType == EWeaponType::Rifle) && CurrentWeapon && WeaponItem)
    {
        // 현재 탄약 수와 최대 탄약 수를 하나의 텍스트로 표시 ("20 / 30" 형식)
        if (CurrentAndMaxAmmoText)
        {
            FString AmmoText = FString::Printf(TEXT("%d / %d"), WeaponItem->CurrentAmmo, WeaponItem->WeaponData.MaxAmmo);
            CurrentAndMaxAmmoText->SetText(FText::FromString(AmmoText));
        }
        
        // 발사 모드 표시
        if (FireModeText)
        {
            ERangeChangeFireMode FireMode = InEquipedWeaponComp->CurrentFireMode;
            FString FireModeStr = (FireMode == ERangeChangeFireMode::Manual) ? TEXT("단발") : TEXT("연발");
            FireModeText->SetText(FText::FromString(FireModeStr));
        }
    }
    // 던지는 무기인 경우 수량 표시
    else if (WeaponType == EWeaponType::ThrowActor && WeaponItem)
    {
        if (ItemQuantityText)
        {
            ItemQuantityText->SetText(FText::AsNumber(WeaponItem->GetQuantity()));
        }
    }
}

void UNS_CurrentWeaponPanel::SetupUIByWeaponType(EWeaponType WeaponType, UNS_InventoryBaseItem* WeaponItem)
{
    // 모든 텍스트 숨기기
    if (CurrentAndMaxAmmoText) CurrentAndMaxAmmoText->SetVisibility(ESlateVisibility::Collapsed);
    if (FireModeText) FireModeText->SetVisibility(ESlateVisibility::Collapsed);
    if (ItemQuantityText) ItemQuantityText->SetVisibility(ESlateVisibility::Collapsed);
    
    // 무기 아이콘 설정
    if (WeaponIcon && WeaponItem)
    {
        UTexture2D* Icon = WeaponItem->GetItemIcon();
        if (Icon)
        {
            WeaponIcon->SetBrushFromTexture(Icon);
            WeaponIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            WeaponIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    else
    {
        if (WeaponIcon) WeaponIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
    
    // 무기 타입에 따라 적절한 UI 요소 표시
    if (WeaponType == EWeaponType::Pistol || WeaponType == EWeaponType::Rifle)
    {
        if (CurrentAndMaxAmmoText) CurrentAndMaxAmmoText->SetVisibility(ESlateVisibility::Visible);
        if (FireModeText) FireModeText->SetVisibility(ESlateVisibility::Visible);
    }
    else if (WeaponType == EWeaponType::ThrowActor)
    {
        if (ItemQuantityText) ItemQuantityText->SetVisibility(ESlateVisibility::Visible);
    }
    // 근접 무기나 Unarmed는 아이콘만 표시
}
