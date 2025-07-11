// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/NS_QuickSlotSlotWidget.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "UI/InGame/NS_QuickSlotKey.h"
#include "UI/InGame/DragItemVisual.h"
#include "UI/InGame/ItemDragDropOperation.h"
#include "UI/InGame/NS_QuickSlotPanel.h"
#include "Character/Components/NS_QuickSlotComponent.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Item/NS_ItemDataStruct.h"


void UNS_QuickSlotSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (UseSelectWeapon)
    {
        UseSelectWeapon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UNS_QuickSlotSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    APlayerController* PC = GetOwningPlayer();
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    ANS_PlayerCharacterBase* Char = Cast<ANS_PlayerCharacterBase>(Pawn);
    if (!Char) return;

    auto* QuickSlotComp = Char->FindComponentByClass<UNS_QuickSlotComponent>();
    auto* WeaponComp = Char->FindComponentByClass<UNS_EquipedWeaponComponent>();
    if (!QuickSlotComp || !WeaponComp) return;

    UNS_InventoryBaseItem* ItemInSlot = QuickSlotComp->GetItemInSlot(SlotIndex);
    UNS_InventoryBaseItem* EquippedItem = WeaponComp->GetCurrentWeaponItem();

    if (ItemInSlot && EquippedItem && ItemInSlot == EquippedItem)
    {
        if (UseSelectWeapon)
        {
            UseSelectWeapon->SetVisibility(ESlateVisibility::Visible);
        }

        // 현재 무기 가져오기 → 원거리 무기인지 확인
        if (auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(WeaponComp->CurrentWeapon))
        {
            int32 CurrentAmmo = RangedWeapon->GetCurrentAmmo();
            int32 MaxAmmo = RangedWeapon->GetMaxAmmo();

            ERangeChangeFireMode CurrentWeaponFireMode = Char->EquipedWeaponComp->CurrentFireMode;
            FText ShotText;

            switch (CurrentWeaponFireMode)
            {
            case ERangeChangeFireMode::Manual:
                ShotText = FText::FromString(FString::Printf(TEXT("단발")));
                break;
            case ERangeChangeFireMode::Auto:
                ShotText = FText::FromString(FString::Printf(TEXT("연발")));
                break;
            default:
                break;
            }

            // UI 갱신
            AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
            AmmoText->SetVisibility(ESlateVisibility::Visible);

            WeaponShotTypeText->SetText(ShotText);
            WeaponShotTypeText->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        AmmoText->SetText(FText::GetEmpty());
        AmmoText->SetVisibility(ESlateVisibility::Collapsed);
        
        WeaponShotTypeText->SetText(FText::GetEmpty());
        WeaponShotTypeText->SetVisibility(ESlateVisibility::Collapsed);

        if (UseSelectWeapon)
        {
            UseSelectWeapon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UNS_QuickSlotSlotWidget::SetAssignedItem(const FNS_ItemDataStruct* ItemData, int32 Quantity)
{
    if (ItemIcon && ItemData)
    {
        ItemIcon->SetBrushFromTexture(ItemData->ItemAssetData.Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }

    if (AmountText)
    {
        if (Quantity > 1)
        {
            AmountText->SetText(FText::AsNumber(Quantity));
            AmountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            AmountText->SetText(FText::GetEmpty());
            AmountText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UNS_QuickSlotSlotWidget::SetSlotIndex(int32 Index)
{
    SlotIndex = Index;
    if (QuickSlotKey)
    {
        QuickSlotKey->SetSlotIndex(Index);
    }
}

void UNS_QuickSlotSlotWidget::ClearAssignedItem()
{
    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (AmountText)
    {
        AmountText->SetText(FText::GetEmpty());
        AmountText->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (WeaponShotTypeText)
    {
        WeaponShotTypeText->SetText(FText::GetEmpty());
        WeaponShotTypeText->SetVisibility(ESlateVisibility::Collapsed);
    }
}