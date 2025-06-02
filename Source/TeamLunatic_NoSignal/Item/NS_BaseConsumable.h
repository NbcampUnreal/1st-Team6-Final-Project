#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseItem.h"
#include "NS_BaseConsumable.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseConsumable : public ANS_BaseItem
{
	GENERATED_BODY()
	
public:
	ANS_BaseConsumable();

	virtual void BeginPlay() override;
	//아이템이 공통적으로 사용할 부분
	
	//음식
	//음료
	//회복용

	//소비 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	USoundBase* UseSound;

	//소비 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	UNiagaraSystem* UseEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	bool bIsBeingUsed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	int32 Maxstack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	int32 Currentstack;

	//아이템 공통 소비 처리 (자식에서 override)
	virtual void Consume();

	//아이템 사용 처리
	virtual void OnUseItem() override;

	virtual void AddItem(int32 Amount);

	virtual void RemoveStack(int32 Amount);

	void PlayUseEffectSound();
};
