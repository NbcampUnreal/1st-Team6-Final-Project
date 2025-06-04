// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory UI/Inventory/InventoryItemSlot.h"
#include "Inventory UI/Inventory/InventoryTooltip.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Inventory UI/Inventory/DragItemVisual.h"
#include "Inventory UI/Inventory/ItemDragDropOperation.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (TooltipClass)
	{
		UInventoryTooltip* Tooltip = CreateWidget<UInventoryTooltip>(this, TooltipClass);
		Tooltip->InventorySlotBeingHovered = this;
		SetToolTip(Tooltip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		switch (ItemReference->ItemType)
		{
			//소모품
		case EItemType::Consumable:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//엔딩조건
		case EItemType::EndingTrigger:
			ItemBorder->SetBrushColor(FLinearColor::Red);
			break;
			//장비
		case EItemType::Equipment:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//제작재료
		case EItemType::Material:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//치료품
		case EItemType::Medical:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
			//기타
		case EItemType::Misc:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
			//도구
		case EItemType::Utility:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		default:;
		}
		ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);

		if (ItemReference->NumericData.isStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// 우클릭 시 아이템 사용
		if (ItemReference)
		{
			if (auto* Player = Cast<ANS_PlayerCharacterBase>(GetOwningPlayerPawn()))
			{
				UE_LOG(LogTemp, Warning, TEXT("[Client] 우클릭 - 서버에 아이템 사용 요청: %s"), *ItemReference->GetName());
				Player->Server_UseInventoryItem(ItemReference); // 서버에 요청

				// 퀵슬롯 자동 배정 로직 추가
				if (Player->QuickSlotPanel)
				{
					if (ItemReference)
					{
						UNS_InventoryBaseItem* ClonedItem = ItemReference->CreateItemCopy();
						if (ClonedItem)
						{
							Player->QuickSlotPanel->AssignToFirstEmptySlot(ClonedItem);
							UE_LOG(LogTemp, Warning, TEXT("[Client] 복제 후 퀵슬롯 등록 시도: %s (%s)"),
								*ClonedItem->GetName(), *ClonedItem->ItemDataRowName.ToString());
						}
					}
				}
			}
		}
		return Reply.Handled();
	}

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));

		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwingInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
