// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/NS_ItemDataStruct.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UNS_InventoryBaseItem;

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

	void BroadcastInventoryUpdate();
	void CleanUpZeroQuantityItems();

	UInventoryComponent();
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UNS_InventoryBaseItem* InputItem);
	
	UFUNCTION(Category = "Inventory")
	UNS_InventoryBaseItem* FindMatchingItem(UNS_InventoryBaseItem* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	UNS_InventoryBaseItem* FindNextItemByID(UNS_InventoryBaseItem* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	UNS_InventoryBaseItem* FindNextPartialStack(UNS_InventoryBaseItem* ItemIn) const;

	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(UNS_InventoryBaseItem* ItemToRemove);
	// 인벤토리에 아이템 삭제시키고 무게 감소시키는 함수 = throw액터에 블루프린트 로직으로 던질때 병 없애고 무게도 없애게 해주고 있음
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveAmountOfItem(UNS_InventoryBaseItem* ItemIn, int32 DesiredAmountToRemove);
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UNS_InventoryBaseItem* ItemIn, const int32 AmountToSplit);

	UFUNCTION(BlueprintCallable)
	void AddNewItem(UNS_InventoryBaseItem* Item, const int32 AmountToAdd);

	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UNS_InventoryBaseItem*> GetInventoryContents() const { return InventoryContents; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", Replicated)
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	int32 InventorySlotsCapacity;
	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", Replicated)
	TArray<TObjectPtr<UNS_InventoryBaseItem>> InventoryContents;

	FItemAddResult HandleNonStackableItems(UNS_InventoryBaseItem* InputItem);
	int32 HandleStackableItems(UNS_InventoryBaseItem* ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UNS_InventoryBaseItem* ItemIn, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UNS_InventoryBaseItem* StackableItem, int32 InitialRequestedAddAmount);
};
