#include "Character/AnimInstance/NS_PlayerAnimInstance.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/Components/NS_EquipedWeaponComponent.h"
#include "Item/NS_BaseRangedWeapon.h"
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
    
    // Turn In Place 업데이트
    UpdateTurnInPlace(DeltaSeconds);
    
    // 이동 중이면 컨트롤러 회전을 계속 활성화
    const bool bIsMoving = PlayerCharacter->GetCharacterMovement()->Velocity.SizeSquared() > KINDA_SMALL_NUMBER;
    if (bIsMoving)
    {
        PlayerCharacter->bUseControllerRotationYaw = true;
        
        // 이동 중에는 Turn In Place 비활성화
        bIsTurningInPlace = false;
        bTurnLeft = false;
        bTurnRight = false;
    }
    else // 이동중이 아니면 컨트롤러 회전 yaw를 비활성화
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
        
        // 더 부드러운 보간을 위해 보간 속도 조정
        const float InterpSpeed = PlayerCharacter->AimSendInterpSpeed;

        // 작은 변화는 무시하여 떨림 방지 (데드존 추가)
        if (FMath::Abs(AimYaw - TargetYaw) > 0.3f)
        {
            AimYaw = FMath::FInterpTo(AimYaw, TargetYaw, DeltaSeconds, InterpSpeed);
        }
        
        if (FMath::Abs(AimPitch - TargetPitch) > 0.3f)
        {
            AimPitch = FMath::FInterpTo(AimPitch, TargetPitch, DeltaSeconds, InterpSpeed);
        }
    }
    else // 원격 컨트롤러는 서버에서 복제된 값 사용
    {
        // 작은 변화는 무시하여 떨림 방지 (데드존 추가)
        if (FMath::Abs(AimYaw - PlayerCharacter->CamYaw) > 0.3f)
        {
            AimYaw = FMath::FInterpTo(AimYaw, PlayerCharacter->CamYaw, DeltaSeconds, 5.0f);
        }
        
        if (FMath::Abs(AimPitch - PlayerCharacter->CamPitch) > 0.3f)
        {
            AimPitch = FMath::FInterpTo(AimPitch, PlayerCharacter->CamPitch, DeltaSeconds, 5.0f);
        }
    }
}

void UNS_PlayerAnimInstance::UpdateTurnInPlace(float DeltaSeconds)
{
    if (!PlayerCharacter) return;
    
    // 플레이어 캐릭터의 Turn In Place 상태 가져오기
    bTurnLeft = PlayerCharacter->TurnLeft;
    bTurnRight = PlayerCharacter->TurnRight;
    
    // 회전 중인지 확인
    bIsTurningInPlace = bTurnLeft || bTurnRight;
    
    // 회전 각도 계산 (애니메이션 블렌드 스페이스에 사용)
    if (bIsTurningInPlace)
    {
        // 왼쪽 회전이면 음수, 오른쪽 회전이면 양수
        float TargetAngle = bTurnLeft ? -95.0f : 95.0f;
        
        // 부드럽게 목표 각도로 보간
        TurnInPlaceAngle = FMath::FInterpTo(TurnInPlaceAngle, TargetAngle, DeltaSeconds, 10.0f);
    }
    else
    {
        // 회전 중이 아니면 각도를 0으로 보간
        TurnInPlaceAngle = FMath::FInterpTo(TurnInPlaceAngle, 0.0f, DeltaSeconds, 10.0f);
    }
}
