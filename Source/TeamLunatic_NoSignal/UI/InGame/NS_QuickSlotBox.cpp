// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_QuickSlotBox.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "UI/InGame/NS_DragItemVisual.h"
#include "UI/InGame/NS_ItemDragDropOperation.h"
#include "UI/InGame/NS_QuickSlotPanel.h"
#include "Character/Components/NS_QuickSlotComponent.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Item/NS_ItemDataStruct.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
 */
void UNS_QuickSlotBox::NativeConstruct()
{
    Super::NativeConstruct();

    // UseSelectWeapon 이미지가 존재하면 초기에는 숨김 처리합니다.
    if (UseSelectWeapon)
    {
        UseSelectWeapon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

/**
 * @brief 매 프레임마다 호출되는 틱 함수입니다.
 * @param MyGeometry 위젯의 지오메트리 정보입니다.
 * @param InDeltaTime 마지막 프레임 이후 경과된 시간입니다.
 *        주로 아이템 정보 업데이트 등 동적인 처리에 사용됩니다.
 */
void UNS_QuickSlotBox::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 플레이어 컨트롤러와 폰을 가져옵니다.
    APlayerController* PC = GetOwningPlayer();
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    ANS_PlayerCharacterBase* Char = Cast<ANS_PlayerCharacterBase>(Pawn);
    // 캐릭터가 유효하지 않으면 함수를 종료합니다.
    if (!Char) return;

    // 퀵 슬롯 컴포넌트와 장착 무기 컴포넌트를 가져옵니다.
    auto* QuickSlotComp = Char->FindComponentByClass<UNS_QuickSlotComponent>();
    auto* WeaponComp = Char->FindComponentByClass<UNS_EquipedWeaponComponent>();
    // 두 컴포넌트 중 하나라도 유효하지 않으면 함수를 종료합니다.
    if (!QuickSlotComp || !WeaponComp) return;

    // 현재 슬롯에 있는 아이템과 현재 장착된 아이템을 가져옵니다.
    UNS_InventoryBaseItem* ItemInSlot = QuickSlotComp->GetItemInSlot(SlotIndex);
    UNS_InventoryBaseItem* EquippedItem = WeaponComp->GetCurrentWeaponItem();

    // 현재 슬롯의 아이템이 존재하고, 장착된 아이템이 존재하며, 두 아이템이 동일할 경우 (즉, 현재 슬롯의 아이템이 장착된 아이템일 경우)
    if (ItemInSlot && EquippedItem && ItemInSlot == EquippedItem)
    {
        // UseSelectWeapon 이미지를 보이게 합니다.
        if (UseSelectWeapon)
        {
            UseSelectWeapon->SetVisibility(ESlateVisibility::Visible);
        }

        // 현재 무기가 원거리 무기인지 확인합니다.
        if (auto* RangedWeapon = Cast<ANS_BaseRangedWeapon>(WeaponComp->CurrentWeapon))
        {
            // 현재 탄약과 최대 탄약을 가져옵니다.
            int32 CurrentAmmo = RangedWeapon->GetCurrentAmmo();
            int32 MaxAmmo = RangedWeapon->GetMaxAmmo();

            // 현재 무기의 발사 모드를 가져옵니다.
            ERangeChangeFireMode CurrentWeaponFireMode = Char->EquipedWeaponComp->CurrentFireMode;
            FText ShotText;

            // 발사 모드에 따라 텍스트를 설정합니다.
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

            // UI (탄약 텍스트 및 발사 모드 텍스트)를 갱신하고 보이게 합니다.
            AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
            AmmoText->SetVisibility(ESlateVisibility::Visible);

            WeaponShotTypeText->SetText(ShotText);
            WeaponShotTypeText->SetVisibility(ESlateVisibility::Visible);
        }
    }
    // 현재 슬롯의 아이템이 장착된 아이템이 아닐 경우
    else
    {
        // 탄약 텍스트와 발사 모드 텍스트를 비우고 숨깁니다.
        AmmoText->SetText(FText::GetEmpty());
        AmmoText->SetVisibility(ESlateVisibility::Collapsed);
        
        WeaponShotTypeText->SetText(FText::GetEmpty());
        WeaponShotTypeText->SetVisibility(ESlateVisibility::Collapsed);

        // UseSelectWeapon 이미지를 숨깁니다.
        if (UseSelectWeapon)
        {
            UseSelectWeapon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

/**
 * @brief 슬롯에 할당된 아이템 정보를 설정합니다.
 * @param ItemData 할당할 아이템의 데이터 구조체 포인터입니다.
 * @param Quantity 아이템의 수량입니다.
 *        아이템 아이콘, 수량 텍스트 등을 업데이트합니다.
 */
void UNS_QuickSlotBox::SetAssignedItem(const FNS_ItemDataStruct* ItemData, int32 Quantity)
{
    // 아이템 아이콘과 아이템 데이터가 유효하면 아이콘을 설정하고 보이게 합니다.
    if (ItemIcon && ItemData)
    {
        ItemIcon->SetBrushFromTexture(ItemData->ItemAssetData.Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }

    // 수량 텍스트가 유효하면 수량에 따라 텍스트를 설정하고 보이거나 숨깁니다.
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

/**
 * @brief 슬롯에 할당된 아이템 정보를 지웁니다.
 *        슬롯을 비활성화 상태로 만들고 표시되는 정보를 초기화합니다.
 */
void UNS_QuickSlotBox::ClearAssignedItem()
{
    // 아이템 아이콘을 숨깁니다.
    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }

    // 수량 텍스트를 비우고 숨깁니다.
    if (AmountText)
    {
        AmountText->SetText(FText::GetEmpty());
        AmountText->SetVisibility(ESlateVisibility::Collapsed);
    }

    // 무기 발사 모드 텍스트를 비우고 숨깁니다.
    if (WeaponShotTypeText)
    {
        WeaponShotTypeText->SetText(FText::GetEmpty());
        WeaponShotTypeText->SetVisibility(ESlateVisibility::Collapsed);
    }
}