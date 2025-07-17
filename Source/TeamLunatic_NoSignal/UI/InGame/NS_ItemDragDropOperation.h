// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "NS_ItemDragDropOperation.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class UNS_InventoryComponent; // 인벤토리 컴포넌트
class UNS_InventoryBaseItem;  // 인벤토리 기본 아이템 클래스

/**
 * @brief 아이템 드래그 앤 드롭 작업을 위한 커스텀 UDragDropOperation 클래스입니다.
 *        드래그 중인 아이템과 해당 아이템이 속한 인벤토리 컴포넌트에 대한 정보를 저장합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_ItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 드래그 중인 아이템에 대한 참조입니다.
	 *        드롭 시 어떤 아이템이 이동되는지 식별하는 데 사용됩니다.
	 */
	UPROPERTY()
	UNS_InventoryBaseItem* SourceItem;

	/**
	 * @brief 드래그 중인 아이템이 원래 속해 있던 인벤토리 컴포넌트에 대한 참조입니다.
	 *        아이템 이동 또는 교환 로직에서 원본 인벤토리를 식별하는 데 사용됩니다.
	 */
	UPROPERTY()
	UNS_InventoryComponent* SourceInventory;
};
