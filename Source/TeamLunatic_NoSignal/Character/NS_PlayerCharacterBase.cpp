#include "Character/NS_PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.H"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Components/NS_EquipedWeaponComponent.h"
#include "Character/Components/NS_StatusComponent.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/Pickup.h"
#include <Net/UnrealNetwork.h>

ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    DefaultWalkSpeed = 500.f;

    SprintSpeedMultiplier = 1.5f;

    // 카메라 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CameraComp->SetupAttachment(GetMesh(), CameraAttachSocketName);
    CameraComp->bUsePawnControlRotation = true;

    // 1인칭용 팔 설정
    FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));
    FirstPersonArms->SetupAttachment(CameraComp);
    FirstPersonArms->bCastDynamicShadow = false;
    FirstPersonArms->CastShadow = false;
    FirstPersonArms->SetOnlyOwnerSee(true); // 플레이어 본인만 보이게 설정 (다른클라이언트는 안보이게)
    
    // 캐릭터 회전 및 이동 방향 설정
    // bUseControllerRotationYaw는 AnimInstance에서 이동여부에따라 이동중이면 true 이동중이 아니면 false로 설정되고있음
    bUseControllerRotationYaw = false; // 초기값은 false로 설정
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    // 스탯 컴포넌트 부착
    StatusComp = CreateDefaultSubobject<UNS_StatusComponent>(TEXT("StatusComponent"));
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    EquipedWeaponComp = CreateDefaultSubobject<UNS_EquipedWeaponComponent>(TEXT("EquipedWeaponComponent"));

    BaseEyeHeight = 74.0f;
    // 인벤토리
    PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
    SetReplicates(true);
    PlayerInventory->SetSlotsCapacity(20);
    PlayerInventory->SetWeightCapacity(50.0f);
}

void ANS_PlayerCharacterBase::DropItem_Server_Implementation(UNS_InventoryBaseItem* ItemToDrop, int32 QuantityToDrop)
{
    if (PlayerInventory->FindMatchingItem(ItemToDrop))
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.bNoFail = true;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        const FVector ForwardOffset = GetActorForwardVector() * 100.0f; // 플레이어 앞 100cm
        const FVector SpawnLocation = GetActorLocation() + ForwardOffset + FVector(0.f, 0.f, 50.f); // 약간 위로 올림
        const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

        const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);
        if (RemovedQuantity <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("DropItem_Server: 제거할 수량이 0 이하입니다."));
            return;
        }

        APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

        Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null"));
    }
}

void ANS_PlayerCharacterBase::DropItem(UNS_InventoryBaseItem* ItemToDrop, const int32 QuantityToDrop)
{
    if (HasAuthority())
    {
        DropItem_Server_Implementation(ItemToDrop, QuantityToDrop);
    }
    else
    {
        DropItem_Server(ItemToDrop, QuantityToDrop); // 클라에서 서버로 요청
    }
}

void ANS_PlayerCharacterBase::Server_UseInventoryItem_Implementation(UNS_InventoryBaseItem* Item)
{
    if (Item)
    {
        Item->OnUseItem(); // 서버에서 처리
    }
}

void ANS_PlayerCharacterBase::Client_NotifyInventoryUpdated_Implementation()
{
    if (PlayerInventory)
    {
        FTimerHandle DelayHandle;
        GetWorldTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([this]()
            {
                PlayerInventory->OnInventoryUpdated.Broadcast();
                UE_LOG(LogTemp, Warning, TEXT("Client_NotifyInventoryUpdated - Inventory 갱신 (지연 호출)"));
            }), 0.05f, false);
    }
}

void ANS_PlayerCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())
    {
        GetMesh()->SetOwnerNoSee(true);          // 전체 메시는 보이지 않게
        FirstPersonArms->SetOnlyOwnerSee(true);  // 팔 메시만 본인(플레이어)이 보이게
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
                ETriggerEvent::Started,
                InteractionComponent,
                &UInteractionComponent::ToggleMenu
            );
        }

        if (InputAttackAction)
        {
            EnhancedInput->BindAction(
            InputAttackAction,
             ETriggerEvent::Started,
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
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsPickUp);  // 아이템줍기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsHit);     // 맞는지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamYaw);    // 카메라 좌/우 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamPitch);  // 카메라 상/하 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAiming);  // 조준중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, PlayerInventory);
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
    StatusComp->AddHealthGauge(-ActualDamage);

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
    if (!Controller) return;
    
    // 카메라 회전 적용
    FVector2D LookInput = Value.Get<FVector2D>(); 
    AddControllerYawInput  (LookInput.X); 
    AddControllerPitchInput(LookInput.Y); 

    // Actor Rotation과 Control Rotation을 Delta를 이용해 Yaw값 추출
    const FRotator ActorRot   = GetActorRotation(); 
    const FRotator ControlRot = Controller->GetControlRotation(); 
    const FRotator DeltaRot   = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot); 

    const float RawYaw   = DeltaRot.Yaw; 
    const float RawPitch = DeltaRot.Pitch; 
    const float DeltaTime = GetWorld()->GetDeltaSeconds();
    
    CamYaw   = FMath::FInterpTo(CamYaw,   DeltaRot.Yaw,   DeltaTime, AimSendInterpSpeed); 
    CamPitch = FMath::FInterpTo(CamPitch, DeltaRot.Pitch, DeltaTime, AimSendInterpSpeed); 

    // InterpTo를 이요해서 부드러운 Yaw/Pitch값을 서버로 전송
    UpdateAim_Server(CamYaw, CamPitch); 
}

void ANS_PlayerCharacterBase::JumpAction(const FInputActionValue& Value)
{
    bool IsJump = Value.Get<bool>(); 
    
    if (IsJump && IsCanJump) 
    {
        Jump(); 
        IsCanJump = false; 

        // 점프한 뒤로 1.3초동안은 점프를 못함
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
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier * SpeedMultiAtStat; 
}

void ANS_PlayerCharacterBase::StopSprint_Server_Implementation(const FInputActionValue& Value)
{
    IsSprint = false; 
    if (GetCharacterMovement()) 
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SpeedMultiAtStat;
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

    EquipedWeaponComp->StartAttack();
}

void ANS_PlayerCharacterBase::StopAttackAction_Server_Implementation(const FInputActionValue& Value)
{
    EquipedWeaponComp->StopAttack();
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
    if(IsAvaliableAiming)
        IsAiming = true; 
}


void ANS_PlayerCharacterBase::StopAimingAction_Server_Implementation(const FInputActionValue& Value)
{
    IsAiming = false; 
}

void ANS_PlayerCharacterBase::ReloadAction_Server_Implementation(const FInputActionValue& Value)
{
	EquipedWeaponComp->Reload();

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

void ANS_PlayerCharacterBase::SwapWeapon(TSubclassOf<ANS_BaseWeapon> WeaponClass)
{
    EquipedWeaponComp->SwapWeapon(WeaponClass); 
}

void ANS_PlayerCharacterBase::AddWeightInventory(float Weight)
{
	if (PlayerInventory)
	{
        //TODO: 인벤토리에 추가무게 넣기
		//PlayerInventory->AddWeightCapacity(Weight);
	}
}

void ANS_PlayerCharacterBase::AddSearchTime(float Multiple)
{
    //TODO: 아이템 찾기의 수색시간 증감(배율)
	//??->AddSerachTime(Multiple)
}

void ANS_PlayerCharacterBase::AddCraftingSpeed(float Multiple)
{
	//TODO: 아이템 제작의 속도 증감(배율)
	//??->AddCraftingSpeed(Multiple);
}
