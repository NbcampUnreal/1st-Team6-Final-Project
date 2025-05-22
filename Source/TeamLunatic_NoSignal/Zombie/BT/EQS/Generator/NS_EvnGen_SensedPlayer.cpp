// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/BT/EQS/Generator/NS_EvnGen_SensedPlayer.h"

#include "AIController.h"
#include "Android/AndroidInputInterface.h"
#include "Perception/AIPerceptionComponent.h"

UNS_EvnGen_SensedPlayer::UNS_EvnGen_SensedPlayer() {}



void UNS_EvnGen_SensedPlayer::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	UObject* Object = QueryInstance.Owner.Get();
	AActor* QueryActor = Cast<AActor>(Object);
	if (!QueryActor) return;
	
	AAIController* AIController = Cast<AAIController>(QueryActor);
	if (!AIController) return;
	UAIPerceptionComponent* PerceptionComp = AIController->GetPerceptionComponent();
	if (!PerceptionComp) return;
	
	TArray<AActor*> SensedActors;
	PerceptionComp->GetCurrentlyPerceivedActors(nullptr, SensedActors);
	for (AActor* Actor : SensedActors)
	{
		if (IsValid(Actor))
		{
			QueryInstance.AddItemData<AActor*>(Actor)();
		}
	}
	
}

