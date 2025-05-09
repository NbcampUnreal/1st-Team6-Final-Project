#include "Character/NS_PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 기본 속도 설정
	DefaultWalkSpeed = 600.f;
	// 캐릭터 달리기 배율
	SprintSpeedMultiplier = 1.5f;

	// 1인칭 카메라 생성 및 Attach
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComp->SetupAttachment(GetMesh(), CameraAttachSocketName);
	CameraComp->bUsePawnControlRotation = true;

	// 컨트롤러 회전 방식 설정
	bUseControllerRotationYaw                        = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ANS_PlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Sub->AddMappingContext(DefaultMappingContext, 0);
		}
	}

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
			&ANS_PlayerCharacterBase::StartCrouch
			);
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
			InputSprintAction, ETriggerEvent::Triggered, this, &ANS_PlayerCharacterBase::StartSprint);
			EnhancedInput->BindAction(
			InputSprintAction, ETriggerEvent::Completed, this, &ANS_PlayerCharacterBase::StopSprint);
		}
	}
}

void ANS_PlayerCharacterBase::MoveAction(const FInputActionValue& Value)
{
	// 에러방지용
	if (!Controller) return;

	// X축 이동
	FVector2D MoveInput = Value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(),MoveInput.X);
	}
	// Y축 이동
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(),MoveInput.Y);
	}
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

		// 타이머핸들로 1.3초동안은 점프 다시 못함
		// TODO : 다른 방법으로 점프 금지를 계산해야할 수 도 있음
		GetWorldTimerManager().SetTimer(
			RestartJumpTime,
			FTimerDelegate::CreateLambda([this](){IsCanJump = true;}),
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
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
}

void ANS_PlayerCharacterBase::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}
