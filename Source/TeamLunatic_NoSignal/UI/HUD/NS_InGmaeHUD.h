// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NS_InGmaeHUD.generated.h"

struct FInteractableData;
struct FNearbyItemInfo;
class UNS_InventoryMainMenu;
class UNS_InteractionWidget;
class UNS_NearbyItemsWidget;
class UNS_InteractionComponent;
class UNS_PlayerWidget;
class ANS_PlayerCharacterBase;
class UNS_QuickSlotPanel;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_InGmaeHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// 플레이어 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_PlayerWidget> PlayerWidgetClass;

	// 인벤토리 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InventoryMainMenu> InventoryMainMenuClass;

	// 상호작용 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InteractionWidget> InteractionWidgetClass;
	
	// 주변 아이템 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_NearbyItemsWidget> NearbyItemsWidgetClass;
	
	// 상호작용 위젯을 반환하는 함수
	UNS_InteractionWidget* GetInteractionWidget() const { return InteractionWidget; }
	
	// 주변 아이템 위젯을 반환하는 함수
	UNS_NearbyItemsWidget* GetNearbyItemsWidget() const { return NearbyItemsWidget; }
	
	// 플레이어 위젯을 반환하는 함수
	UNS_PlayerWidget* GetPlayerWidget() const { return PlayerWidget; }
	
	// 메뉴가 보이는지 여부
	bool bIsMenuVisible;

	ANS_InGmaeHUD();

	// 메뉴를 표시하는 함수
	void DisplayMenu();
	// 메뉴를 숨기는 함수
	void HideMenu();
	// 인벤토리 위젯을 여는 함수
	void OpenInventoryWidget();
	
	// UI 업데이트 함수들
	void UpdatePlayerHealth(int32 CurrentHealth, int32 MaxHealth);
	void UpdatePlayerStamina(int32 CurrentStamina, int32 MaxStamina);
	void ShowTipMessage(const FText& TipText);
	void HideTipMessage();
	void SetCrosshairVisibility(bool bVisible);
	
	// 플레이어 참조 설정
	void SetPlayerCharacter(ANS_PlayerCharacterBase* InPlayerCharacter);

	// 상호작용 위젯을 표시하는 함수
	void ShowInteractionWidget();
	// 상호작용 위젯을 숨기는 함수
	void HideInteractionWidget();
	// 상호작용 위젯을 업데이트하는 함수
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;
	
	// 주변 아이템 위젯을 표시하는 함수
	void ShowNearbyItemsWidget();
	// 주변 아이템 위젯을 숨기는 함수
	void HideNearbyItemsWidget();
	// 주변 아이템 위젯을 업데이트하는 함수
	void UpdateNearbyItemsWidget(const TArray<FNearbyItemInfo>& NearbyItems);
	
	// 인터랙션 컴포넌트 설정
	void SetInteractionComponent(UNS_InteractionComponent* InInteractionComponent);

protected:
	// 인벤토리 메뉴 위젯
	UPROPERTY()
	UNS_InventoryMainMenu* InventoryMainMenuWidget;

	// 상호작용 위젯
	UPROPERTY()
	UNS_InteractionWidget* InteractionWidget;
	
	// 주변 아이템 위젯
	UPROPERTY()
	UNS_NearbyItemsWidget* NearbyItemsWidget;
	
	// 플레이어 위젯
	UPROPERTY()
	UNS_PlayerWidget* PlayerWidget;
	
	
	// 인터랙션 컴포넌트 참조
	UPROPERTY()
	UNS_InteractionComponent* InteractionComponent;
	
	// 플레이어 참조
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	// 게임이 시작될 때 호출되는 함수
	virtual void BeginPlay() override;
	
	// 주변 아이템 목록이 업데이트되었을 때 호출되는 함수
	UFUNCTION()
	void OnNearbyItemsUpdated();
};
