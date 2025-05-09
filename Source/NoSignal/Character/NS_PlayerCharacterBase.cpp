#include "Character/NS_PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "NS_PlayerController.h"


ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultWalkSpeed = 600.f; // 기본 속도 (에디터에서 조정 가능)
	SprintSpeedMultiplier = 1.5f; // 스프린트 속도 배율
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 0.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ANS_PlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANS_PlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANS_PlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ANS_PlayerController* PlayerController = Cast<ANS_PlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
				PlayerController->MoveAction, 
				ETriggerEvent::Triggered,     
				this,                         
				&ANS_PlayerCharacterBase::MoveAction
				);
			}
			
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction, 
					ETriggerEvent::Triggered,   
					this,                       
					&ANS_PlayerCharacterBase::JumpAction 
				);
			}
			
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction, 
					ETriggerEvent::Triggered,    
					this,                       
					&ANS_PlayerCharacterBase::LookAction         
				);
			}
			
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,    
					this,                        
					&ANS_PlayerCharacterBase::StartSprintAction   
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,    
					this,                        
					&ANS_PlayerCharacterBase::StopSprintAction   
				);
				
			}
			
			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Triggered,
					this,
					&ANS_PlayerCharacterBase::CrouchAction
				);
			}
		}
	}
}

void ANS_PlayerCharacterBase::MoveAction(const FInputActionValue& value)
{
	// 에러방지용
	if (!Controller) return;

	// X축 이동
	FVector2D MoveInput = value.Get<FVector2D>();
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

void ANS_PlayerCharacterBase::LookAction(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ANS_PlayerCharacterBase::JumpAction(const FInputActionValue& value)
{
	bool IsJump = value.Get<bool>();
	
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

void ANS_PlayerCharacterBase::CrouchAction(const FInputActionValue& value)
{
	Crouch();

	UE_LOG(LogTemp, Warning, TEXT("IsCrouching: %d"), GetCharacterMovement()->IsCrouching());
}

void ANS_PlayerCharacterBase::StartSprintAction(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
	}
}

void ANS_PlayerCharacterBase::StopSprintAction(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}