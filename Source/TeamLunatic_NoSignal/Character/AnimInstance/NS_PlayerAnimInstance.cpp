#include "Character/AnimInstance/NS_PlayerAnimInstance.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNS_PlayerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    if (APawn* PlayChar = TryGetPawnOwner())
        PlayerCharacter = Cast<ANS_PlayerCharacterBase>(PlayChar);
}

void UNS_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!PlayerCharacter) return;

    // AimOffset 업데이트
    UpdateAimOffset(DeltaSeconds);
    
    // 이동 중이면 컨트롤러 회전을 계속 활성화
    const bool bIsMoving = PlayerCharacter->GetCharacterMovement()->Velocity.SizeSquared() > KINDA_SMALL_NUMBER;
    if (bIsMoving)
    {
        PlayerCharacter->bUseControllerRotationYaw = true;
    }
    else // 이동중이 아니명 컨트롤러 회전 yaw를 비활성화
    {
        PlayerCharacter->bUseControllerRotationYaw = false;
    }
}
void UNS_PlayerAnimInstance::UpdateAimOffset(float DeltaSeconds)
{
    if (!PlayerCharacter) return;

    // 로컬 플레이어라면 부드러운 AimYaw와 AimPitch값을 사용
    if (PlayerCharacter->IsLocallyControlled())
    {
        const FRotator ControlRot = PlayerCharacter->GetController()
                                    ? PlayerCharacter->GetController()->GetControlRotation()
                                    : FRotator::ZeroRotator;
        const FRotator ActorRot   = PlayerCharacter->GetActorRotation();
        const FRotator DeltaRot   = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);

        const float TargetYaw   = DeltaRot.Yaw;
        const float TargetPitch = DeltaRot.Pitch;
        const float InterpSpeed = PlayerCharacter->AimSendInterpSpeed;

        AimYaw   = FMath::FInterpTo(AimYaw,   TargetYaw,   DeltaSeconds, InterpSpeed);
        AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaSeconds, InterpSpeed);
    }
    else // 원격 컨트롤러는 서버에서 복제된 값 사용
    {
        AimYaw   = PlayerCharacter->CamYaw;
        AimPitch = PlayerCharacter->CamPitch;
    }
}

