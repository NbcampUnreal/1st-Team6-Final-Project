// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NS_InventoryHUD.generated.h"

struct FInteractableData;
class UNS_InventoryMainMenu;
class UNS_InteractionWidget;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_InventoryHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// 인벤토리 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InventoryMainMenu> InventoryMainMenuClass;

	// 상호작용 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InteractionWidget> InteractionWidgetClass;

	// 상호작용 위젯을 반환하는 함수
	UNS_InteractionWidget* GetInteractionWidget() const { return InteractionWidget; }
	// 메뉴가 보이는지 여부
	bool bIsMenuVisible;

	ANS_InventoryHUD();

	// 메뉴를 표시하는 함수
	void DisplayMenu();
	// 메뉴를 숨기는 함수
	void HideMenu();
	// 인벤토리 위젯을 여는 함수
	void OpenInventoryWidget();

	// 상호작용 위젯을 표시하는 함수
	void ShowInteractionWidget();
	// 상호작용 위젯을 숨기는 함수
	void HideInteractionWidget();
	// 상호작용 위젯을 업데이트하는 함수
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	// 인벤토리 메뉴 위젯
	UPROPERTY()
	UNS_InventoryMainMenu* InventoryMainMenuWidget;

	// 상호작용 위젯
	UPROPERTY()
	UNS_InteractionWidget* InteractionWidget;

	// 게임이 시작될 때 호출되는 함수
	virtual void BeginPlay() override;
};
