// NS_PlayerAnimInstance.cpp

#include "Character/AnimInstance/NS_PlayerAnimInstance.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h" // CharacterMovementComponent를 위해 추가

void UNS_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
    
	APawn* OwningPawn = TryGetPawnOwner();
	if (OwningPawn)
	{
		PlayerCharacter = Cast<ANS_PlayerCharacterBase>(OwningPawn);
	}
}

void UNS_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!PlayerCharacter) return;

	// 1) 목표 Yaw/Pitch 결정 (로컬 vs 원격)
	float TargetYaw   = 0.f;
	float TargetPitch = 0.f;

	// 캐릭터의 이동 속도 확인 (AnimInstance에서 직접 접근)
	const bool bIsMoving = PlayerCharacter->GetCharacterMovement()->Velocity.SizeSquared() > KINDA_SMALL_NUMBER;

	if (PlayerCharacter->IsLocallyControlled())
	{
		// 로컬 컨트롤러는 매프레임 직접 계산
		const FRotator ControlRot = PlayerCharacter->GetController()
									? PlayerCharacter->GetController()->GetControlRotation()
									: FRotator::ZeroRotator;
		const FRotator ActorRot   = PlayerCharacter->GetActorRotation();
		const FRotator DeltaRot   = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot); //

		TargetYaw   = DeltaRot.Yaw; //
		TargetPitch = DeltaRot.Pitch; //

		// 2) Turn In Place 로직 (정지 상태일 때만 작동)
		if (!bIsMoving) // 정지 상태
		{
            // Turn In Place 시작 조건
			if (FMath::Abs(TargetYaw) > TurnInPlaceAngleThreshold) //
			{
				if (TargetYaw > 0)
				{
					bIsTurningRight = true;
					bIsTurningLeft = false;
				}
				else
				{
					bIsTurningLeft = true;
					bIsTurningRight = false;
				}
                // Turn In Place가 시작되면 캐릭터의 Yaw를 AnimInstance가 제어하도록 요청
                SetCharacterUseControllerRotationYaw(false);
			}
            // Turn In Place 종료 조건
            // (몸통이 시선을 충분히 따라갔거나, 방향이 반대로 바뀔 때)
            else if ((bIsTurningLeft && TargetYaw >= -5.0f) || (bIsTurningRight && TargetYaw <= 5.0f)) // -5 ~ 5 사이면 거의 0에 가깝다고 간주
            {
                bIsTurningLeft = false;
                bIsTurningRight = false;
                // Turn In Place가 끝나면 다시 컨트롤러 Yaw를 따르도록 요청
                SetCharacterUseControllerRotationYaw(true);
            }
		}
        else // 이동 중
        {
            // 이동 중에는 Turn In Place 상태 해제
            bIsTurningLeft = false;
            bIsTurningRight = false;
            // 이동 중에는 몸통이 컨트롤러 회전을 따르도록 설정
            SetCharacterUseControllerRotationYaw(true);
        }
	}
	else // 원격 컨트롤러는 서버에서 복제된 값 사용
	{
		TargetYaw   = PlayerCharacter->CamYaw; //
		TargetPitch = PlayerCharacter->CamPitch; //
	}

	// 3) 보간
	AimYaw   = FMath::FInterpTo(AimYaw, TargetYaw, DeltaSeconds, PlayerCharacter->AimSendInterpSpeed); //
	AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaSeconds, PlayerCharacter->AimSendInterpSpeed); //
}

// 캐릭터의 bUseControllerRotationYaw를 설정하는 함수 구현
void UNS_PlayerAnimInstance::SetCharacterUseControllerRotationYaw(bool bNewValue)
{
    if (PlayerCharacter)
    {
        PlayerCharacter->bUseControllerRotationYaw = bNewValue;
    }
}