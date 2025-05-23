// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class ANS_BaseWeapon;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	TAR_NoItemAdded UMETA(DisplayName = "No Item added"),
	TAR_PartialAmountItemAdded UMETA(DisplayName = "Partial of Item added"),
	TAR_AllItemAdded UMETA(DisplayName = "All of Item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() : 
		ActualAmountAdded(0), 
		OperationResult(EItemAddResult::TAR_NoItemAdded), 
		ResultMessage(FText::GetEmpty())
	{};

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::TAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	};
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::TAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::TAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnInventoryUpdated OnInventoryUpdated;

	UInventoryComponent();

	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(ANS_BaseWeapon* InputItem);
	
	UFUNCTION(Category = "Inventory")
	ANS_BaseWeapon* FindMatchingItem(ANS_BaseWeapon* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	ANS_BaseWeapon* FindNextItemByID(ANS_BaseWeapon* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	ANS_BaseWeapon* FindNextPartialStack(ANS_BaseWeapon* ItemIn) const;

	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(ANS_BaseWeapon* ItemToRemove);
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(ANS_BaseWeapon* ItemIn, int32 DesiredAmountToRemove);
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(ANS_BaseWeapon* ItemIn, const int32 AmountToSplit);

	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<ANS_BaseWeapon*> GetInventoryContents() const { return InventoryContents; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	int32 InventorySlotsCapacity;
	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<ANS_BaseWeapon>> InventoryContents;

	FItemAddResult HandleNonStackableItems(ANS_BaseWeapon* InputItem, int32 RequestedAddAmount);
	int32 HandleStackableItems(ANS_BaseWeapon* ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(ANS_BaseWeapon* ItemIn, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(ANS_BaseWeapon* StackableItem, int32 InitialRequestedAddAmount);

	void AddNewItem(ANS_BaseWeapon* Item, const int32 AmountToAdd);
};
