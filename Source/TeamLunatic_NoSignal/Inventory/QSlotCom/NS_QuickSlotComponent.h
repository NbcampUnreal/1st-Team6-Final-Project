// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NS_QuickSlotComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMLUNATIC_NOSIGNAL_API UNS_QuickSlotComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    UNS_QuickSlotComponent();
    void BeginPlay();
    FOnQuickSlotUpdated QuickSlotUpdated;

    UFUNCTION(BlueprintCallable)
    void AssignToSlot(int32 Index, class UNS_InventoryBaseItem* Item);

    bool AssignToFirstEmptySlot(UNS_InventoryBaseItem* Item);

    UFUNCTION(BlueprintCallable)
    class UNS_InventoryBaseItem* GetItemInSlot(int32 Index) const;

    UFUNCTION(BlueprintCallable)
    bool RemoveItem(class UNS_InventoryBaseItem* Item);

    void CleanInvalidSlots();

    const TArray<TObjectPtr<UNS_InventoryBaseItem>>& GetQuickSlots() const { return QuickSlots; }

    bool IsItemAlreadyAssigned(UNS_InventoryBaseItem* Item) const;

    void BroadcastSlotUpdate();

    UPROPERTY()
    int32 CurrentQuickSlotIndex = INDEX_NONE;

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentSlotIndex() const { return CurrentQuickSlotIndex; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentSlotIndex(int32 NewIndex);
    
    UFUNCTION()
    void OnRep_QuickSlots();
    
    UPROPERTY(EditDefaultsOnly, Category = "QuickSlot")
    int32 InitialSlotCount = 5;

    // 특정 아이템이 있는 슬롯 인덱스 찾기
    UFUNCTION(BlueprintCallable, Category = "QuickSlot")
    int32 FindSlotIndexForItem(UNS_InventoryBaseItem* Item) const;

    // 퀵슬롯에 할당된 아이템 총 개수 반환
    UFUNCTION(BlueprintCallable, Category = "QuickSlot")
    int32 GetTotalAssignedItems() const;

    // 최대 슬롯 수 반환
    UFUNCTION(BlueprintCallable, Category = "QuickSlot")
    int32 GetMaxSlots() const { return InitialSlotCount; }
    
protected:
    UPROPERTY(ReplicatedUsing = OnRep_QuickSlots)
    TArray<TObjectPtr<UNS_InventoryBaseItem>> QuickSlots;

    bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};