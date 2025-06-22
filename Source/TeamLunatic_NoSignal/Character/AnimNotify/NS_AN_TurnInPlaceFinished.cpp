#include "Character/AnimNotify/NS_AN_TurnInPlaceFinished.h"
#include "Character/NS_PlayerCharacterBase.h"

UNS_AN_TurnInPlaceFinished::UNS_AN_TurnInPlaceFinished()
{
}

void UNS_AN_TurnInPlaceFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    // 플레이어 캐릭터에 메쉬에서 Get Owner로 현재 컨트롤러 캐릭터를 캐스트해서
    if (MeshComp && MeshComp->GetOwner())
    {
        ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(MeshComp->GetOwner());
        if (PlayerCharacter)
        {
            // Turn In Place 완료 함수 호출
            PlayerCharacter->OnTurnInPlaceFinished();
        }
    }
}

FString UNS_AN_TurnInPlaceFinished::GetNotifyName_Implementation() const
{
    // 노티파이 이름 반환
    return TEXT("Turn In Place Finished");
}