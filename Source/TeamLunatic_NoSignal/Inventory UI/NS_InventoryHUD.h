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
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InventoryMainMenu> InventoryMainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UNS_InteractionWidget> InteractionWidgetClass;

	UNS_InteractionWidget* GetInteractionWidget() const { return InteractionWidget; }
	bool bIsMenuVisible;

	ANS_InventoryHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	UPROPERTY()
	UNS_InventoryMainMenu* InventoryMainMenuWidget;

	UPROPERTY()
	UNS_InteractionWidget* InteractionWidget;

	virtual void BeginPlay() override;
};
