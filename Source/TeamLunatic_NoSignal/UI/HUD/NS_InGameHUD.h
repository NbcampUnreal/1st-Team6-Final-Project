// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NS_InGameHUD.generated.h"

struct FInteractableData;
struct FNearbyItemInfo;
class UNS_InventoryMainWidget;
class UNS_InteractionPanel;
class UNS_NearbyItemsPanel;
class UNS_InteractionComponent;
class UNS_PlayerWidget;
class ANS_PlayerCharacterBase;
class UNS_QuickSlotPanel;
class UNS_LevelMapWidget;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_InGameHUD : public AHUD
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	// 인터랙션 컴포넌트 참조
	UPROPERTY()
	UNS_InteractionComponent* InteractionComponent;
	
	// 플레이어 참조
	UPROPERTY()
	ANS_PlayerCharacterBase* PlayerCharacter;

	
	// 플레이어 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_PlayerWidget> PlayerWidgetClass;
	// 플레이어 위젯
	UPROPERTY()
	UNS_PlayerWidget* PlayerWidget;
	
	
	// 인벤토리 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InventoryMainWidget> InventoryMainClass;
	// 인벤토리 메뉴 위젯
	UPROPERTY()
	UNS_InventoryMainWidget* InventoryMainWidget;


	// 레벨지도 메뉴 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_LevelMapWidget> LevelMapWidgetClass;
	// 레벨지도 위젯
	UPROPERTY()
	UNS_LevelMapWidget* LevelMapWidget;
	
public:

	UNS_PlayerWidget* GetPlayerWidget() const { return PlayerWidget; }
	UNS_InventoryMainWidget* GetInventoryMainWidget() const { return InventoryMainWidget; }
	UNS_LevelMapWidget* GetLevelMapWidget() const { return LevelMapWidget; }
	
	// 해당하는 위젯은 열고 나머지 위젯들은 닫아주는 함수
	void ShowWidget(UUserWidget* OpenWidget);
};
