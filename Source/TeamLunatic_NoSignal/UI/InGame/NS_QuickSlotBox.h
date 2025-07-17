// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_QuickSlotBox.generated.h"

// 전방 선언: 클래스 및 구조체 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UNS_InventoryBaseItem; // 인벤토리 기본 아이템 클래스
class UImage;               // UMG 이미지 위젯
class UTextBlock;           // UMG 텍스트 블록 위젯
class UNS_QuickSlotKey;     // 퀵 슬롯 키 위젯
class UNS_DragItemVisual;      // 드래그 아이템 시각화 위젯
struct FNS_ItemDataStruct;  // 아이템 데이터 구조체

/**
 * @brief 퀵 슬롯의 개별 슬롯을 나타내는 위젯입니다.
 *        아이템 아이콘, 수량, 탄약 정보 등을 표시하며, 퀵 슬롯 패널에 포함됩니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotBox : public UUserWidget
{
    GENERATED_BODY()
	
public:
    /**
     * @brief 위젯이 생성될 때 호출되는 초기화 함수입니다.
     *        주로 위젯의 초기 상태 설정 및 바인딩에 사용됩니다.
     */
    virtual void NativeConstruct() override;

    /**
     * @brief 매 프레임마다 호출되는 틱 함수입니다.
     * @param MyGeometry 위젯의 지오메트리 정보입니다.
     * @param InDeltaTime 마지막 프레임 이후 경과된 시간입니다.
     *        주로 아이템 정보 업데이트 등 동적인 처리에 사용됩니다.
     */
    void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

    /**
     * @brief 슬롯에 할당된 아이템 정보를 설정합니다.
     * @param ItemData 할당할 아이템의 데이터 구조체 포인터입니다.
     * @param Quantity 아이템의 수량입니다.
     *        아이템 아이콘, 수량 텍스트 등을 업데이트합니다.
     */
    void SetAssignedItem(const FNS_ItemDataStruct* ItemData, int32 Quantity);
	
    /**
     * @brief 슬롯에 할당된 아이템 정보를 지웁니다.
     *        슬롯을 비활성화 상태로 만들고 표시되는 정보를 초기화합니다.
     */
    void ClearAssignedItem();

    /**
     * @brief 아이템의 아이콘을 표시하는 이미지 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UImage* ItemIcon;

    /**
     * @brief 현재 선택된 무기를 표시하는 이미지 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    class UImage* UseSelectWeapon;

    /**
     * @brief 아이템의 수량을 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩될 수 있으며, 필수는 아닙니다.
     */
    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* AmountText;

    /**
     * @brief 무기의 현재 탄약 수를 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* AmmoText;

    /**
     * @brief 무기의 발사 모드(예: 단발, 연사)를 표시하는 텍스트 블록 위젯입니다.
     *        블루프린트에서 바인딩됩니다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* WeaponShotTypeText;

    /**
     * @brief 현재 슬롯에 할당된 인벤토리 아이템 객체입니다.
     *        런타임에 할당됩니다.
     */
    UPROPERTY()
    UNS_InventoryBaseItem* AssignedItem;

    /**
     * @brief 이 슬롯의 고유 인덱스입니다.
     *        퀵 슬롯 패널 내에서 슬롯을 식별하는 데 사용됩니다.
     */
    UPROPERTY()
    int32 SlotIndex;
};