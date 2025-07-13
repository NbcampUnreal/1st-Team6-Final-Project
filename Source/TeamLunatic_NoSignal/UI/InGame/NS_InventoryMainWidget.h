// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "NS_InventoryMainWidget.generated.h"

// 전방 선언: 클래스 정의 전에 미리 선언하여 컴파일러에게 존재를 알립니다.
class ANS_PlayerCharacterBase; // 플레이어 캐릭터 클래스
class UNS_NearbyItemsPanel; // 주변 아이템 위젯 클래스

/**
 * @brief 인벤토리 메인 메뉴를 나타내는 위젯입니다.
 *        주로 인벤토리 패널, 장비 슬롯 등 인벤토리 관련 UI를 포함하고 관리합니다.
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_InventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;
	
protected:
	// 위젯이 초기화될 때 호출되는 함수
	virtual void NativeOnInitialized() override;

	// 위젯이 생성될 때 호출되는 함수
	virtual void NativeConstruct() override;

	// 드래그 앤 드롭 작업이 이 위젯 위에서 드롭될 때 호출되는 함수
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};