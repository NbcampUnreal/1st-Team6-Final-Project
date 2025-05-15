#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Debug/NS_DebugStatusWidget.h"  // 디버그용 차후 삭제 가능
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Net/UnrealNetwork.h>

ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultWalkSpeed = 400.f;
    SprintSpeedMultiplier = 1.5f;

    // 카메라 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CameraComp->SetupAttachment(GetMesh(), CameraAttachSocketName);
    CameraComp->bUsePawnControlRotation = true; // 카메라는 컨트롤러 회전을 따라감

    // 캐릭터 회전 및 이동 방향 설정
    bUseControllerRotationYaw = false; // 기존에 마우스 회전에 따라 캐릭터 몸 이동 해제
    GetCharacterMovement()->bOrientRotationToMovement = false; // 좌/우 이동 값을 벡터로 이동이 아닌 회전으로 변경하는거 해제
    GetCharacterMovement()->bUseControllerDesiredRotation = false; // 정확하게 이게 뭔지 모르겠는데 OrientRotationToMovement를 실행하면 이것도 같이 켜줘야하는걸로 기억

    // 스탯 컴포넌트 부착
    StatusComp = CreateDefaultSubobject<UNS_StatusComponent>(TEXT("StatusComponent"));
}

void ANS_PlayerCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // 디버그 위젯 생성 ======================== 차후 삭제필요
    if (DebugWidgetClass && Controller)
    {
        if (APlayerController* PC = Cast<APlayerController>(Controller))
        {
            DebugWidgetInstance = CreateWidget<UNS_DebugStatusWidget>(PC, DebugWidgetClass);
            if (DebugWidgetInstance)
            {
                DebugWidgetInstance->AddToViewport();
            }
        }
    }

    // 입력 매핑 컨텍스트 등록
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Sub->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // 기본 속도 설정
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
    }
}

void ANS_PlayerCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Controller)
        return;

    // 이동 중인지를 Velocity로 체크하고 이동중이 아니면 실행안함
    if (GetCharacterMovement()->Velocity.IsNearlyZero())
        return;

    // 캐릭터가 바라봐야 할 목표 방향
    const float TargetYaw = Controller->GetControlRotation().Yaw;
    // 캐릭터가 회전하는 값
    const FRotator Current = GetActorRotation();
    // 캐릭터가 최종적으로 회전해야하는 목표 값
    const FRotator Desired(0.f, TargetYaw, 0.f);
    // 현재 캐릭터의 회전 값인 Current에서 목표 값인 Desired으로 CharacterTurnSpeed에 저장된 회전 속도로 회전함 
    const FRotator NewRot = FMath::RInterpTo(Current, Desired, DeltaTime, CharacterTurnSpeed);
    // 계산된 NewRot값을 캐릭터에 실제로 적용시켜 회전
    SetActorRotation(NewRot);
}

void ANS_PlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (InputMoveAction)
        {
            EnhancedInput->BindAction(
                InputMoveAction,
                ETriggerEvent::Triggered,
                this,
                &ANS_PlayerCharacterBase::MoveAction);
        }
        

        if (InputLookAction)
        {
            EnhancedInput->BindAction(
                InputLookAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::LookAction);
        }

        if (InputJumpAction)
        {
            EnhancedInput->BindAction(
                InputJumpAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::JumpAction);
        }

        if (InputCrouchAction)
        {
            EnhancedInput->BindAction(
            InputCrouchAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartCrouch);
            EnhancedInput->BindAction(
            InputCrouchAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopCrouch);
        }
        
        if (InputSprintAction)
        {
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartSprint_Server);
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopSprint_Server);
        }

        if (InputKickAction)
        {
            EnhancedInput->BindAction(
            InputKickAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::KickAction_Server);
        }
    }
}

void ANS_PlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsKick);    // 발차기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsSprint); // 달리기 변수

}

void ANS_PlayerCharacterBase::SetMovementLockState(bool bLock)
{
    if (auto* MoveComp = GetCharacterMovement())
    {
        if (bLock)
            MoveComp->DisableMovement();
        else
            MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
    }
}

float ANS_PlayerCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    StatusComp->ChangeHealthGauge(-DamageAmount);


    return ActualDamage;
}

void ANS_PlayerCharacterBase::OnDeath()
{
}

void ANS_PlayerCharacterBase::MoveAction(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (!GetCharacterMovement()) return;

    FVector2D MoveInput = Value.Get<FVector2D>();
    if (!FMath::IsNearlyZero(MoveInput.X))
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    if (!FMath::IsNearlyZero(MoveInput.Y))
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
}

void ANS_PlayerCharacterBase::LookAction(const FInputActionValue& Value)
{
    FVector2D LookInput = Value.Get<FVector2D>();
    FRotator ControlRot = Controller->GetControlRotation();
    AddControllerPitchInput(LookInput.Y);
    
    float ActorYaw = GetActorRotation().Yaw;
    float NewYaw = ControlRot.Yaw + LookInput.X;

    // 좌/우(Yaw값) 각독 제한 -90 ~ +90까지 허용
    float RelativeYaw = FMath::ClampAngle(NewYaw - ActorYaw, -90.f, 90.f);
    ControlRot.Yaw = ActorYaw + RelativeYaw;

    Controller->SetControlRotation(ControlRot);
}

void ANS_PlayerCharacterBase::JumpAction(const FInputActionValue& Value)
{
    bool IsJump = Value.Get<bool>();
    
    if (IsJump && IsCanJump)
    {
        Jump();
        IsCanJump = false;

        FTimerHandle RestartJumpTime;
        GetWorldTimerManager().SetTimer(
            RestartJumpTime,
            FTimerDelegate::CreateLambda([this]() { IsCanJump = true; }),
            1.3f,
            false
        );
    }
}

void ANS_PlayerCharacterBase::StartCrouch(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) { return; }
    
    Crouch();
}

void ANS_PlayerCharacterBase::StopCrouch(const FInputActionValue& Value)
{
    UnCrouch();
}

void ANS_PlayerCharacterBase::StartSprint_Server_Implementation(const FInputActionValue& Value)
{
    StartSprint_Multicast_Implementation();
}
void ANS_PlayerCharacterBase::StartSprint_Multicast_Implementation()
{
    IsSprint = true;
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
}

void ANS_PlayerCharacterBase::StopSprint_Server_Implementation(const FInputActionValue& Value)
{
    StopSprint_Multicast();
}
void ANS_PlayerCharacterBase::StopSprint_Multicast_Implementation()
{
    IsSprint = false;
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}


void ANS_PlayerCharacterBase::KickAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) {return;}
    KickAction_Multicast();
}
void ANS_PlayerCharacterBase::KickAction_Multicast_Implementation()
{
    if(HasAuthority())
    {
        IsKick = true;

        // 1.2초간 실 후 IsKick변수는 false로 변경
        FTimerHandle RestKickTime;
        GetWorldTimerManager().SetTimer(
            RestKickTime,
            FTimerDelegate::CreateLambda([this]() { IsKick = false; }),
            1.2f,
            false
        );
    }
}