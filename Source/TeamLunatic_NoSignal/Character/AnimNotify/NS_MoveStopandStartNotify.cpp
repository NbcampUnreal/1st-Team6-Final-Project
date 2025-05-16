#include "NS_MoveStopandStartNotify.h"
#include "Character/NS_PlayerCharacterBase.h"

void UNS_MoveStopandStartNotify::NotifyBegin(USkeletalMeshComponent* MeshComp,
											  UAnimSequenceBase* Animation,
											  float TotalDuration)
{
	if (auto* Char = Cast<ANS_PlayerCharacterBase>(MeshComp->GetOwner()))
		Char->SetMovementLockState_Server(true);
}

void UNS_MoveStopandStartNotify::NotifyEnd(USkeletalMeshComponent* MeshComp,
											UAnimSequenceBase* Animation)
{
	if (auto* Char = Cast<ANS_PlayerCharacterBase>(MeshComp->GetOwner()))
		Char->SetMovementLockState_Server(false);
}