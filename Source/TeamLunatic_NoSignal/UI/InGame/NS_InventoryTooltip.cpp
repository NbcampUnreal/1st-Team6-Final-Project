// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/NS_InventoryTooltip.h"
#include "Item/NS_InventoryBaseItem.h"
#include "UI/InGame/NS_InventoryItemSlot.h"

/**
 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
 *        주로 툴팁에 표시될 아이템 정보를 설정하고 UI를 업데이트합니다.
 */
void UNS_InventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	// 현재 마우스가 올려진 인벤토리 슬롯의 아이템 참조를 가져옵니다.
	const UNS_InventoryBaseItem* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	// 아이템 타입에 따라 툴팁의 표시 방식을 변경합니다.
	switch (ItemBeingHovered->ItemType)
	{
		// 소모품 타입
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString(TEXT("소모품"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

		// 엔딩 조건 타입
	case EItemType::EndingTrigger:
		ItemType->SetText(FText::FromString(TEXT("엔딩 조건"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

		// 장비 타입
	case EItemType::Equipment:
		ItemType->SetText(FText::FromString(TEXT("장비"))); // 아이템 타입 텍스트 설정
		MaxStack->SetVisibility(ESlateVisibility::Collapsed);             // 최대 스택 텍스트 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Visible);   // 무기 타입 관련 UI 표시

		// 무기 타입에 따라 추가 정보 표시
		switch (ItemBeingHovered->WeaponType)
		{
		case EWeaponType::Knife:
			WeaponType->SetText(FText::FromString(TEXT("근접 무기"))); // 무기 타입 텍스트 설정
			DamageValue->SetVisibility(ESlateVisibility::Visible);      // 피해량 값 표시
			break;
		case EWeaponType::Rifle:
			WeaponType->SetText(FText::FromString(TEXT("원거리 무기"))); // 무기 타입 텍스트 설정
			DamageValue->SetVisibility(ESlateVisibility::Visible);      // 피해량 값 표시
			break;
		case EWeaponType::Pistol:
			WeaponType->SetText(FText::FromString(TEXT("원거리 무기"))); // 무기 타입 텍스트 설정
			DamageValue->SetVisibility(ESlateVisibility::Visible);      // 피해량 값 표시
			break;
		case EWeaponType::Magazine:
			WeaponType->SetText(FText::FromString(TEXT("탄창"))); // 무기 타입 텍스트 설정
			DamageValue->SetVisibility(ESlateVisibility::Collapsed); // 피해량 값 숨김
			break;
		case EWeaponType::Ammo:
			WeaponType->SetText(FText::FromString(TEXT("탄약"))); // 무기 타입 텍스트 설정
			DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 피해량 관련 UI 숨김
			MaxStack->SetVisibility(ESlateVisibility::Visible);         // 최대 스택 텍스트 표시
			break;
		default:
			WeaponType->SetText(FText::FromString(TEXT("기타 무기"))); // 기본 무기 타입 텍스트 설정
			DamageValue->SetVisibility(ESlateVisibility::Visible);      // 피해량 값 표시
			MaxStack->SetVisibility(ESlateVisibility::Visible);         // 최대 스택 텍스트 표시
			break;
		}
		break;

		// 제작 재료 타입
	case EItemType::Material:
		ItemType->SetText(FText::FromString(TEXT("제작 재료"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

		// 치료품 타입
	case EItemType::Medical:
		ItemType->SetText(FText::FromString(TEXT("치료 아이템"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

		// 기타 타입
	case EItemType::Misc:
		ItemType->SetText(FText::FromString(TEXT("기타"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

		// 도구 타입
	case EItemType::Utility:
		ItemType->SetText(FText::FromString(TEXT("도구"))); // 아이템 타입 텍스트 설정
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;

	default:
		ItemType->SetText(FText::FromString(TEXT("알 수 없음"))); // 알 수 없는 타입일 경우
		WeaponTypeHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 무기 타입 관련 UI 숨김
		DamageHorizontal->SetVisibility(ESlateVisibility::Collapsed);     // 피해량 관련 UI 숨김
		UsageText->SetVisibility(ESlateVisibility::Collapsed);            // 사용법 텍스트 숨김
		break;
	}

	// 아이템의 공통 정보를 설정합니다.
	ItemName->SetText(ItemBeingHovered->TextData.ItemName);             // 아이템 이름 설정
	UsageText->SetText(ItemBeingHovered->TextData.InteractionText);     // 사용법 텍스트 설정
	ItemDescription->SetText(ItemBeingHovered->TextData.ItemDescription); // 아이템 설명 설정
	StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight())); // 스택된 아이템의 총 무게 설정
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->WeaponData.Damage));   // 무기 피해량 설정

	// 무게 정보를 문자열로 구성하여 표시합니다.
	const FString WeightInfo = {FString(TEXT("무게: ")) + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };

	StackWeight->SetText(FText::FromString(WeightInfo));

	// 아이템이 스택 가능한지 여부에 따라 최대 스택 정보를 표시하거나 숨깁니다.
	if (ItemBeingHovered->NumericData.isStackable)
	{
		// 최대 스택 정보를 문자열로 구성하여 표시합니다.
		const FString StackInfo = {FString(TEXT("최대 수량: ")) + FString::FromInt(ItemBeingHovered->NumericData.MaxStack) };

		MaxStack->SetText(FText::FromString(StackInfo));
		MaxStackHorizontal->SetVisibility(ESlateVisibility::Visible); // 최대 스택 관련 UI 표시
	}
	else
	{
		MaxStackHorizontal->SetVisibility(ESlateVisibility::Collapsed); // 최대 스택 관련 UI 숨김
	}
}
