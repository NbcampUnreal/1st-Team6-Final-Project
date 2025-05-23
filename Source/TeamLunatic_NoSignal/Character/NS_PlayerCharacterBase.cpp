#include "Character/NS_PlayerCharacterBase.h"
#include "Character/Debug/NS_DebugStatusWidget.h"  // 디버그용 차후 삭제
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/InteractionInterface.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Components/NS_EquipedWeaponComponent.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Interaction/Component/InteractionComponent.h"

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
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    EquipedWeaponComp = CreateDefaultSubobject<UNS_EquipedWeaponComponent>(TEXT("EquipedWeaponComponent"));

    BaseEyeHeight = 74.0f;
    // 인벤토리
    PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
    PlayerInventory->SetSlotsCapacity(20);
    PlayerInventory->SetWeightCapacity(50.0f);

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

    // 캐릭터가 이동중이라면 몸체회전시키는변수인 CharacterTurnSpeed = 5값으로 카메라가 바라보는 중앙으로 몸을 회전시킴
    if (!GetCharacterMovement()->Velocity.IsNearlyZero())
    {
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

        if (InteractAction)
        {
            EnhancedInput->BindAction(
                InteractAction,
                ETriggerEvent::Started,
               InteractionComponent,
                &UInteractionComponent::BeginInteract
            );

             EnhancedInput->BindAction(
                 InteractAction,
                 ETriggerEvent::Completed,
                 InteractionComponent,
                 &UInteractionComponent::EndInteract
             );
        }

        if (ToggleMenuAction)
        {
            EnhancedInput->BindAction(
                ToggleMenuAction,
                ETriggerEvent::Triggered,
                InteractionComponent,
                &UInteractionComponent::ToggleMenu
            );
        }

        if (InputAttackAction)
        {
            EnhancedInput->BindAction(
            InputAttackAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartAttackAction_Server);
            EnhancedInput->BindAction(
            InputAttackAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopAttackAction_Server);
        }

        if (InteractAction)
        {
            EnhancedInput->BindAction(
            InteractAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::PickUpAction_Server);
        }

        if (InputAimingAction)
        {
            EnhancedInput->BindAction(
            InputAimingAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartAimingAction_Server);
            
            EnhancedInput->BindAction(
           InputAimingAction,
            ETriggerEvent::Completed,
             this,
              &ANS_PlayerCharacterBase::StopAimingAction_Server);
        }

        if (InputReloadAction)
        {
            EnhancedInput->BindAction(
            InputReloadAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::ReloadAction_Server);
        }
    }
}

void ANS_PlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsKick);    // 발차기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsSprint);  // 달리기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAttack);  // 공격 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsPickUp);  // 아이템줍기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsChange);  // ================================= 나중에에 삭제해야함
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsHit);     // 맞는지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamYaw);    // 카메라 좌/우 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamPitch);  // 카메라 상/하 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAiming);  // 조준중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsReload);  // 장전중인지 확인 변수
}

void ANS_PlayerCharacterBase::SetMovementLockState_Server_Implementation(bool bLock)
{
	SetMovementLockState_Multicast(bLock);
}

void ANS_PlayerCharacterBase::SetMovementLockState_Multicast_Implementation(bool bLock)
{
    if (auto* MoveComp = GetCharacterMovement())
    {
        if (bLock)
            MoveComp->DisableMovement();
        else
            MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
    }
}

float ANS_PlayerCharacterBase::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (!HasAuthority() || ActualDamage <= 0.f)
        return ActualDamage;

    // 캐릭터 체력 감소
    StatusComp->ChangeHealthGauge(-ActualDamage);

    IsHit = true;
    
    // IsHit 타이머핸들 람다로 0.5초간 실행
    FTimerHandle ResetHitTime;
    GetWorldTimerManager().SetTimer(
        ResetHitTime,
        [this]() { IsHit = false;},
        0.5f,
        false
        );

    // 캐릭터 체력이 0이면 죽음 애니메이션 실행
    if (StatusComp->Health <= 0.f)
    {
        PlayDeath_Server();
    }

    return ActualDamage;
}

//////////////////////////////////액션 처리 함수들///////////////////////////////////
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
    
    // 상/하 회전
    AddControllerPitchInput(LookInput.Y);
    
    // 좌/우 회전
    FRotator ControlRot = Controller->GetControlRotation();
    float ActorYaw = GetActorRotation().Yaw;
    float NewYaw = ControlRot.Yaw + LookInput.X;
    
    // 좌/우(Yaw값) 각도 제한 -90 ~ +90까지 허용
    float RelativeYaw = FMath::ClampAngle(NewYaw - ActorYaw, -90.f, 90.f);
    ControlRot.Yaw = ActorYaw + RelativeYaw;
    
    // 컨트롤러 회전에 반영하여 카메라와 캐릭터 조준 축 업데이트 ------------- (자세한 원리 부가 설명 필요)
    Controller->SetControlRotation(ControlRot);

    const float NewCamYaw   = RelativeYaw;      
    const float NewCamPitch = FMath::ClampAngle( ControlRot.Pitch, -90.f, 90.f);

    UpdateAim_Server(NewCamYaw, NewCamPitch);
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
	//점프 중이거나 발차기 중일 때는 앉지 않음
    if (GetCharacterMovement()->IsFalling() || IsKick) { return; }
    
    Crouch();
}

void ANS_PlayerCharacterBase::StopCrouch(const FInputActionValue& Value)
{
    UnCrouch();
}

void ANS_PlayerCharacterBase::StartSprint_Server_Implementation(const FInputActionValue& Value)
{
    IsSprint = true;
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
}

void ANS_PlayerCharacterBase::StopSprint_Server_Implementation(const FInputActionValue& Value)
{
    IsSprint = false;
    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

void ANS_PlayerCharacterBase::KickAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) {return;}

    IsKick = true;

    // 1.2초간 실행 후 IsKick변수는 false로 변경
    FTimerHandle ResettKickTime;
    GetWorldTimerManager().SetTimer(
        ResettKickTime,
        FTimerDelegate::CreateLambda([this]() { IsKick = false; }),
        1.2f,
        false
    );
}

void ANS_PlayerCharacterBase::StartAttackAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) {return;}

    IsAttack = true;
}

void ANS_PlayerCharacterBase::StopAttackAction_Server_Implementation(const FInputActionValue& Value)
{
    IsAttack = false;
}

void ANS_PlayerCharacterBase::PickUpAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) {return;}

    IsPickUp = true;

    // 1.0초간 실행 후 IsPickUp변수는 false로 변경
    FTimerHandle ResetPickUpTime;
    GetWorldTimerManager().SetTimer(
    ResetPickUpTime,
    FTimerDelegate::CreateLambda([this]() { IsPickUp = false; }),
    1.0f,
    false
    );
}

void ANS_PlayerCharacterBase::StartAimingAction_Server_Implementation(const FInputActionValue& Value)
{
    IsAiming = true;
}


void ANS_PlayerCharacterBase::StopAimingAction_Server_Implementation(const FInputActionValue& Value)
{
    IsAiming = false;
}

void ANS_PlayerCharacterBase::ReloadAction_Server_Implementation(const FInputActionValue& Value)
{
    IsReload = true;

    // 2.5초간 실행 후 IsReload변수는 false로 변경
    FTimerHandle ResetPickUpTime;
    GetWorldTimerManager().SetTimer(
    ResetPickUpTime,
    FTimerDelegate::CreateLambda([this]() { IsReload = false; }),
    2.5f,
    false
    );
}
//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////

void ANS_PlayerCharacterBase::PlayDeath_Server_Implementation()
{
    PlayDeath_Multicast();
}

void ANS_PlayerCharacterBase::PlayDeath_Multicast_Implementation()
{
    DetachFromControllerPendingDestroy();
	
    GetCharacterMovement()->DisableMovement();

    GetMesh()->SetCollisionProfileName("Ragdoll");
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->WakeAllRigidBodies();
    GetMesh()->bBlendPhysics = true;
    SetLifeSpan(5.f);
}

// 클라이언트면 서버로 클라이언트 자신에 Yaw값과 Pitch값을 서버로 전송
void ANS_PlayerCharacterBase::UpdateAim_Server_Implementation(float NewCamYaw, float NewCamPitch)
{
    CamYaw   = NewCamYaw;
    CamPitch = NewCamPitch;
}

void ANS_PlayerCharacterBase::SwapWeapon(TSubclassOf<ANS_BaseMeleeWeapon> WeaponClass)
{
    EquipedWeaponComp->SwapWeapon(WeaponClass);
}