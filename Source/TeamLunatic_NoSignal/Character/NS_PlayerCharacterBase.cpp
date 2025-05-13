// NS_PlayerCharacterBase.cpp
#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Debug/NS_DebugStatusWidget.h"  // 디버그용 차후 삭제 가능
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultWalkSpeed = 600.f;
    SprintSpeedMultiplier = 1.5f;

    // 카메라 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CameraComp->SetupAttachment(GetMesh(), CameraAttachSocketName);
    CameraComp->bUsePawnControlRotation = true;

    // 회전 및 이동 방향 설정
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

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
}

void ANS_PlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (InputMoveAction)
            EnhancedInput->BindAction(
                InputMoveAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::MoveAction
               );

        if (InputLookAction)
            EnhancedInput->BindAction(
                InputLookAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::LookAction
               );

        if (InputJumpAction)
            EnhancedInput->BindAction(
                InputJumpAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::JumpAction
               );

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
               &ANS_PlayerCharacterBase::StopCrouch
               );
        }
        if (InputSprintAction)
        {
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartSprint);
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopSprint
               );
        }
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
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
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
    Crouch();
}

void ANS_PlayerCharacterBase::StopCrouch(const FInputActionValue& Value)
{
    UnCrouch();
}

void ANS_PlayerCharacterBase::StartSprint(const FInputActionValue& Value)
{
    IsSprint = true;
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
}

void ANS_PlayerCharacterBase::StopSprint(const FInputActionValue& Value)
{
    GetCharacterMovement()->IsMovingOnGround();
    IsSprint = false;
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}
