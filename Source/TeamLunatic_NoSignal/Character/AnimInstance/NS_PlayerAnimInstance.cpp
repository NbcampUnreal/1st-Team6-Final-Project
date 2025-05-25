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

    // 1) AimOffset 업데이트
    UpdateAimOffset(DeltaSeconds);

    // 2) 이동 중이면 컨트롤러 회전 항상 ON, 턴 플래그 리셋
    const bool bIsMoving = PlayerCharacter->GetCharacterMovement()
                                ->Velocity.SizeSquared() > KINDA_SMALL_NUMBER;
    if (bIsMoving)
    {
        bIsTurningRight = bIsTurningLeft = false;
        PlayerCharacter->bUseControllerRotationYaw = true;
    }
    else
    {
        if (!bIsTurningInPlace)
        {
            PlayerCharacter->bUseControllerRotationYaw = false;
        }
        TurnInPlace(DeltaSeconds);
    }
}
void UNS_PlayerAnimInstance::UpdateAimOffset(float DeltaSeconds)
{
    if (!PlayerCharacter) return;

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
        AimYaw   = FMath::FInterpTo(AimYaw, PlayerCharacter->CamYaw, DeltaSeconds, PlayerCharacter->AimSendInterpSpeed); //
        AimPitch = FMath::FInterpTo(AimPitch, PlayerCharacter->CamPitch, DeltaSeconds, PlayerCharacter->AimSendInterpSpeed); //
    }
}

void UNS_PlayerAnimInstance::TurnInPlace(float DeltaSeconds)
{
    if (!PlayerCharacter) return;

    // 1) 오른쪽 턴 시작 체크 (A == +90, 오차 ±2)
    if (FMath::IsNearlyEqual(AimYaw, TurnInPlaceActivationAngle, 2.f))
    {
        bIsTurningInPlace = true;
        bIsTurningRight = true;
        bIsTurningLeft  = false;
        PlayerCharacter->bUseControllerRotationYaw = true;
    }
    // 2) 왼쪽 턴 시작 체크 (A == -90, 오차 ±2)
    else if (FMath::IsNearlyEqual(AimYaw, -TurnInPlaceActivationAngle, 2.f))
    {
        bIsTurningInPlace = true;
        bIsTurningRight  = true;
        bIsTurningLeft = false;
        PlayerCharacter->bUseControllerRotationYaw = true;
    }

    // // 3) 턴 해제 조건: AimYaw가 -2 ~ +2 사이로 돌아왔을 때
    // if (AimYaw > -TurnInPlaceDeactivationThreshold && AimYaw < TurnInPlaceDeactivationThreshold)
    // {
    //     bIsTurningInPlace = false;
    //     bIsTurningRight = false;
    //     bIsTurningLeft  = false;
    //     PlayerCharacter->bUseControllerRotationYaw = false;
    // }
    if (FMath::Abs(AimYaw) < TurnInPlaceDeactivationThreshold)
    {
        bIsTurningInPlace = false;
        bIsTurningRight = false;
        bIsTurningLeft  = false;
        PlayerCharacter->bUseControllerRotationYaw = false;
    }
}

