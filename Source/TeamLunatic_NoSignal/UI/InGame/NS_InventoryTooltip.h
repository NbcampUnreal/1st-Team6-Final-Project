// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "NS_InventoryTooltip.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UTextBlock;       // UMG 텍스트 블록 위젯
class UNS_InventoryItemSlot; // 인벤토리 아이템 슬롯 위젯

/**
 * @brief 인벤토리 아이템에 마우스를 올렸을 때 표시되는 툴팁 위젯입니다.
 *        아이템의 상세 정보(이름, 종류, 피해량, 설명, 스택 등)를 보여줍니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 현재 마우스가 올려진 인벤토리 아이템 슬롯에 대한 참조입니다.
	 *        이 툴팁이 어떤 아이템 슬롯의 정보를 표시하는지 나타냅니다.
	 */
	UPROPERTY(VisibleAnywhere)
	UNS_InventoryItemSlot* InventorySlotBeingHovered;

	/**
	 * @brief 아이템의 이름을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	/**
	 * @brief 아이템의 종류(예: 무기, 소모품)를 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;


	/**
	 * @brief 무기 아이템의 종류(예: 소총, 권총)를 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponType;

	/**
	 * @brief 무기 종류 텍스트를 포함하는 수평 박스 위젯입니다.
	 *        블루프린트에서 바인딩되며, 무기일 경우에만 보이도록 설정될 수 있습니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WeaponTypeHorizontal;

	/**
	 * @brief 무기 아이템의 피해량 값을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;

	/**
	 * @brief 피해량 텍스트를 포함하는 수평 박스 위젯입니다.
	 *        블루프린트에서 바인딩되며, 무기일 경우에만 보이도록 설정될 수 있습니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* DamageHorizontal;

	/**
	 * @brief 아이템의 사용법이나 효과를 설명하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;

	/**
	 * @brief 아이템의 상세 설명을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	/**
	 * @brief 아이템의 최대 스택 가능 수량을 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStack;

	/**
	 * @brief 최대 스택 텍스트를 포함하는 수평 박스 위젯입니다.
	 *        블루프린트에서 바인딩되며, 스택 가능한 아이템일 경우에만 보이도록 설정될 수 있습니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* MaxStackHorizontal;


	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* SellValue;

	/**
	 * @brief 아이템의 무게를 표시하는 텍스트 블록 위젯입니다.
	 *        블루프린트에서 바인딩됩니다.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackWeight;

protected:
	/**
	 * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
	 *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
	 */
	virtual void NativeConstruct() override;
};
