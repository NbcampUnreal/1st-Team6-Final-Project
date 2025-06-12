// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie/Animations/Notify/NS_AnimNotify_CallTracker.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/AIController/NS_AIController.h"
#include "Zombie/AIController/NS_ChaserController.h"


UNS_AnimNotify_CallTracker::UNS_AnimNotify_CallTracker() : ScreamSound(nullptr), MaxHeardRange(3000.f), Loudness(1.f)
{

}

void UNS_AnimNotify_CallTracker::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	if (TrackedActorBP)
	{
		UGameplayStatics::GetAllActorsOfClass(MeshComp->GetWorld(),TrackedActorBP, TrackerActors);
	}
	
	ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(MeshComp->GetOwner());
	if (!Zombie) return;
	ANS_AIController* Controller = Cast<ANS_AIController>(Zombie->GetController());
	if (!Controller) return;
	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	if (!BlackboardComponent) return;

	UObject* Key =  BlackboardComponent->GetValueAsObject("TargetActor");
	if (!Key) return;
	ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(Key);
	if (!Player) return;
	
	
	for (AActor* Chacer : TrackerActors)
	{
		APawn* pawn = Cast<APawn>(Chacer);
		if (!pawn) continue;
		ANS_ChaserController* ChacerController = Cast<ANS_ChaserController>(pawn->GetController());
		
		if (!ChacerController) return;
		
		ChacerController->SetChaseTarget(Player, 60.f);
	}

	if (ScreamSound)
	{
		if (Zombie->HasAuthority())
		{
			Zombie->Multicast_PlaySound(ScreamSound);
		}
		Zombie->Server_PlaySound(ScreamSound);
	}
	UAISense_Hearing::ReportNoiseEvent(MeshComp->GetWorld(), Zombie->GetActorLocation(),Loudness,Zombie, MaxHeardRange);
}
