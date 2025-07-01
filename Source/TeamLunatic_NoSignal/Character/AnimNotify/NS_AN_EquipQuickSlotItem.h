// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NS_AN_EquipQuickSlotItem.generated.h"

UCLASS(meta = (DisplayName = "NS Equip QuickSlot Item"))
class TEAMLUNATIC_NOSIGNAL_API UNS_AN_EquipQuickSlotItem : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    UNS_AN_EquipQuickSlotItem();

    // AnimNotify 오버라이드
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
    
    // 노티파이 이름 설정
    virtual FString GetNotifyName_Implementation() const override;
};