#include "Item/NS_BaseConsumableItem.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

UNS_BaseConsumableItem::UNS_BaseConsumableItem()
{
}

void UNS_BaseConsumableItem::OnUseItem(ANS_PlayerCharacterBase* Character)
{
	if (!Character) return;

	UNS_StatusComponent* State = Character->StatusComp;
	if (State)
	{
		State->AddHealthGauge(ItemStates.HealAmount);
		State->AddStamina(ItemStates.StaminaRecovery);
	}

	

	if (ConsumableItemAssetData.UseSound)
	{
		UGameplayStatics::PlaySound2D(Character, ConsumableItemAssetData.UseSound);
	}

	//if (ConsumableItemAssetData.UseEffect)
	//{
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	//		Character->GetWorld(),
	//		ConsumableItemAssetData.UseEffect,
	//		Character->GetActorLocation()
	//	);
	//}
}


