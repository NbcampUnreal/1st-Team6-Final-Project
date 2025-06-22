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
#include "Character/ThrowActor/NS_ThrowActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/Pickup.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include <Net/UnrealNetwork.h>
#include "Inventory/QSlotCom/NS_QuickSlotComponent.h"

ANS_PlayerCharacterBase::ANS_PlayerCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    DefaultWalkSpeed = 500.f;

    SprintSpeedMultiplier = 1.5f;

    // 스프링 암 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 0.f;
    SpringArmComp->bUsePawnControlRotation = true;
    
    // 카메라 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
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

    // 스탯 컴포넌트
    StatusComp = CreateDefaultSubobject<UNS_StatusComponent>(TEXT("StatusComponent"));
    // 상호작용 컴포넌트
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    // 장착 무기 컴포넌트
    EquipedWeaponComp = CreateDefaultSubobject<UNS_EquipedWeaponComponent>(TEXT("EquipedWeaponComponent"));

    BaseEyeHeight = 74.0f;
    // 인벤토리
    PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
    SetReplicates(true);
    PlayerInventory->SetSlotsCapacity(20);
    PlayerInventory->SetWeightCapacity(50.0f);

    // 퀵슬롯 
    QuickSlotComponent = CreateDefaultSubobject<UNS_QuickSlotComponent>(TEXT("QuickSlotComponent"));
    QuickSlotComponent->SetIsReplicated(true);

    // 스팟라이트 컴포넌트 
    FlashlightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightComponent"));
    // 카메라에 부착
    FlashlightComponent->SetupAttachment(CameraComp);
    FlashlightComponent->SetRelativeRotation(FRotator::ZeroRotator);
    FlashlightComponent->SetVisibility(true); 
    FlashlightComponent->SetIntensity(8000.0f);
    FlashlightComponent->SetOuterConeAngle(30.0f);
    FlashlightComponent->SetInnerConeAngle(15.0f);
    FlashlightComponent->SetAttenuationRadius(2000.0f);
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
    
    // 기본 퀵슬롯는 1번부터 시작되도록 
    CurrentQuickSlotIndex = 0;
    if (QuickSlotComponent)
    {
        QuickSlotComponent->SetCurrentSlotIndex(CurrentQuickSlotIndex);
    }
}

void ANS_PlayerCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 로컬 플레이어인 경우에만 Turn In Place 업데이트
    if (IsLocallyControlled())
    {
        // Turn In Place 업데이트
        if (bIsTurningInPlace)
        {
            UpdateTurnInPlace(DeltaTime);
        }
        // Yaw 리셋 업데이트
        else if (bIsResettingYaw)
        {
            UpdateYawReset(DeltaTime);
        }
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

        if (InteractAction)
        {
            EnhancedInput->BindAction(
            InteractAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::PickUpAction_Server
               );
        }

        if (InputAimingAction)
        {
            EnhancedInput->BindAction(
            InputAimingAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::StartAimingAction_Server
               );
            
            EnhancedInput->BindAction(
           InputAimingAction,
            ETriggerEvent::Completed,
             this,
              &ANS_PlayerCharacterBase::StopAimingAction_Server
              );
        }

        if (InputFlashlightAction)
        {
            EnhancedInput->BindAction(
                InputFlashlightAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::ToggleFlashlight
                );
        }
        
        if (InputQuickSlot1)
        {
            EnhancedInput->BindAction(
                InputQuickSlot1,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::QuickSlot1Selected
                );
        }

        if (InputQuickSlot2)
        {
            EnhancedInput->BindAction(
                InputQuickSlot2,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::QuickSlot2Selected
                );
        }

        if (InputQuickSlot3)
        {
            EnhancedInput->BindAction(
                InputQuickSlot3,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::QuickSlot3Selected
                );
        }

        if (InputQuickSlot4)
        {
            EnhancedInput->BindAction(
                InputQuickSlot4,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::QuickSlot4Selected
                );
        }

        if (InputQuickSlot5)
        {
            EnhancedInput->BindAction(
                InputQuickSlot5,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::QuickSlot5Selected
                );
        }
    }
}

void ANS_PlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_PlayerCharacterBase, IsSprint);            // 달리기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsPickUp);            // 아이템줍기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsHit);               // 맞는지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamYaw);              // 카메라 좌/우 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamPitch);            // 카메라 상/하 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAiming);            // 조준중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsReload);            // 장전중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnLeft);            // 몸을 왼쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnRight);           // 몸을 오른쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, NowFire);             // 사격시 몸전체Mesh 사격 애니메이션 재생 용 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, bFlashlightOnOff);    // 헤드램프 키고 끄는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, PlayerInventory);
    DOREPLIFETIME(ANS_PlayerCharacterBase, QuickSlotComponent);
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsChangeAnim);        // 퀵슬롯 눌렀을때 무기 장착하는 애니메이션 재생 용 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsDead);	             // 캐릭터가 죽었는지 확인 변수
}

void ANS_PlayerCharacterBase::SetMovementLockState_Server_Implementation(bool bLock)
{
	SetMovementLockState_Multicast(bLock);
}

void ANS_PlayerCharacterBase::SetMovementLockState_Multicast_Implementation(bool bLock)
{
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            {
                if (bLock)
                {
                    Subsystem->RemoveMappingContext(DefaultMappingContext);
                }
                else
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }
    }
}

float ANS_PlayerCharacterBase::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    // 이미 죽은 상태라면 데미지를 받지 않음
    if (IsDead)
    {
        return 0.f;
    }
    
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
        [this]()
        {
            // 캐릭터가 있다면 IsHit을 false로 설정
            if (IsValid(this)) //
            {
                IsHit = false; //
            }
        },
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
    AddControllerYawInput(LookInput.X * LookMagnification); 
    AddControllerPitchInput(LookInput.Y * LookMagnification); 

    // Actor Rotation과 Control Rotation을 Delta를 이용해 Yaw값 추출
    const FRotator ActorRot = GetActorRotation(); 
    const FRotator ControlRot = Controller->GetControlRotation(); 
    const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);
    
    const float DeltaTime = GetWorld()->GetDeltaSeconds();
    
    // 회전 중이 아니고 Yaw 리셋 중이 아닐 때만 CamYaw 업데이트
    if (!bIsTurningInPlace && !bIsResettingYaw)
    {
        // 마우스 입력이 있을 때만 CamYaw 업데이트 (마우스 입력이 없으면 이전 값 유지)
        if (FMath::Abs(LookInput.X) > KINDA_SMALL_NUMBER)
        {
            CamYaw = DeltaRot.Yaw;
        }
        
        // 회전 시작 조건 확인
        if (!TurnLeft && !TurnRight)
        {
            if (FMath::Abs(CamYaw) >= TurnInPlaceThreshold)
            {
                // 회전 시작 - 현재 Yaw 값 저장
                CurrentTurnYaw = CamYaw;
                LastTurnYaw = CamYaw;
                bIsTurningInPlace = true;
                bIsResettingYaw = false;
                
                // 왼쪽/오른쪽 회전 설정
                bool bNewTurnLeft = CamYaw < 0;
                bool bNewTurnRight = CamYaw > 0;
                
                // bUseControllerDesiredRotation 활성화
                GetCharacterMovement()->bUseControllerDesiredRotation = true;
                
                // 서버에 상태 업데이트 요청
                if (HasAuthority())
                {
                    // 서버에서 직접 설정하고 멀티캐스트
                    TurnLeft = bNewTurnLeft;
                    TurnRight = bNewTurnRight;
                    Multicast_UpdateTurnInPlaceState(bNewTurnLeft, bNewTurnRight, true);
                }
                else
                {
                    // 클라이언트에서는 서버에 요청
                    Server_UpdateTurnInPlaceState(bNewTurnLeft, bNewTurnRight, true);
                }
            }
        }
    }
    
    CamPitch = FMath::FInterpTo(CamPitch, DeltaRot.Pitch, DeltaTime, AimSendInterpSpeed); 

    // 카메라 회전 정보를 서버로 전송 (손전등 회전도 함께 처리됨)
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
    if (GetCharacterMovement()->IsFalling()) { return; } 
    
    Crouch(); 
}

void ANS_PlayerCharacterBase::StopCrouch(const FInputActionValue& Value)
{
    UnCrouch(); 
}

void ANS_PlayerCharacterBase::StartSprint_Server_Implementation(const FInputActionValue& Value)
{
    if (StatusComp->CheckEnableSprint())
    {
        IsSprint = true;
        if (GetCharacterMovement())
            GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier * SpeedMultiAtStat;
    }
    else
    {
        IsSprint = false;
        if (GetCharacterMovement())
            GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SpeedMultiAtStat;
    }
}

void ANS_PlayerCharacterBase::StopSprint_Server_Implementation(const FInputActionValue& Value)
{
    IsSprint = false; 
    if (GetCharacterMovement()) 
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SpeedMultiAtStat;
}

void ANS_PlayerCharacterBase::PickUpAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) { return; } // 낙하 중에는 아이템 줍기 불가
    if (IsPickUp) { return; } // 이미 아이템 줍기 중이면 무시

    if (UInteractionComponent* InteractComp = FindComponentByClass<UInteractionComponent>())
    {
        const TScriptInterface<IInteractionInterface>& CurrentTarget = InteractComp->GetCurrentInteractable();

        if (!CurrentTarget.GetObject())
        {
            UE_LOG(LogTemp, Warning, TEXT("상호작용 대상 없음"));
            return;
        }
        
        // 상호작용 실행 (Pickup 클래스의 Interact 함수 호출)
        IInteractionInterface::Execute_Interact(CurrentTarget.GetObject(), this);
    }
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

// 퀵슬롯 선택 함수들
void ANS_PlayerCharacterBase::QuickSlot1Selected() { HandleQuickSlotKeyInput(1); }
void ANS_PlayerCharacterBase::QuickSlot2Selected() { HandleQuickSlotKeyInput(2); }
void ANS_PlayerCharacterBase::QuickSlot3Selected() { HandleQuickSlotKeyInput(3); }
void ANS_PlayerCharacterBase::QuickSlot4Selected() { HandleQuickSlotKeyInput(4); }
void ANS_PlayerCharacterBase::QuickSlot5Selected() { HandleQuickSlotKeyInput(5); }
//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////


void ANS_PlayerCharacterBase::PlayDeath_Server_Implementation()
{
    if (UWorld* World = GetWorld())
    {
        ANS_GameModeBase* BaseGameMode = Cast<ANS_GameModeBase>(UGameplayStatics::GetGameMode(World));
        if (BaseGameMode)
        {
            BaseGameMode->OnPlayerCharacterDied(this);

            if (AController* OwningController = GetController())
            {
                if (ANS_MainGamePlayerState* PS = Cast<ANS_MainGamePlayerState>(OwningController->PlayerState))
                {
                    PS->bIsAlive = false; 
                    UE_LOG(LogTemp, Warning, TEXT("Player %s PlayerState set to Dead."), *PS->GetPlayerName());
                }
            }
        }
    }
    PlayDeath_Multicast();
}

void ANS_PlayerCharacterBase::PlayDeath_Multicast_Implementation()
{
    // 캐릭터가 죽었으면 IsDead변수를 true로 변경해서 애니메이션 몽타주가 1번만 재생되도록 구현했음
    IsDead = true;
    
    // 컨트롤러에서 분리시키고
    DetachFromControllerPendingDestroy(); 

    // 무브먼트 없애고
    GetCharacterMovement()->DisableMovement(); 

    // 기존 레그돌은 부르르 떨려서 대신 몽타주 재생으로 바꿨음
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
    
    // 월드에서 사라지는 시간은 30초로
    SetLifeSpan(30.f); 
}

void ANS_PlayerCharacterBase::DropItem_Server_Implementation(UNS_InventoryBaseItem* ItemToDrop, int32 QuantityToDrop)
{
    if (PlayerInventory->FindMatchingItem(ItemToDrop))
    {
        if (EquipedWeaponComp && EquipedWeaponComp->GetCurrentWeaponItem() == ItemToDrop)
        {
            EquipedWeaponComp->UnequipWeapon();
        }
        QuickSlotComponent->RemoveItem(ItemToDrop);
        Client_NotifyQuickSlotUpdated();
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

void ANS_PlayerCharacterBase::Client_NotifyQuickSlotUpdated_Implementation()
{
    if (QuickSlotComponent)
    {
        QuickSlotComponent->BroadcastSlotUpdate(); // UI 갱신
    }
}

void ANS_PlayerCharacterBase::UseThrowableItem_Internal(int32 Index)
{
    if (HasAuthority())
    {
        HandleUseThrowableItem(Index); // 싱글플레이 / 서버 권한
    }
    else
    {
        Server_UseThrowableItem(Index); // 클라 → 서버
    }
}

void ANS_PlayerCharacterBase::Server_UseThrowableItem_Implementation(int32 Index)
{
    if (!IsThrow)
    {
        IsThrow = true;  // 애니메이션 실행 상태 플래그
        UE_LOG(LogTemp, Warning, TEXT("[Server_UseThrowableItem] 슬롯 %d 애니메이션 시작 준비"), Index);
    }
}

void ANS_PlayerCharacterBase::HandleUseThrowableItem(int32 Index)
{
    if (!QuickSlotComponent || !PlayerInventory) return;
    QuickSlotComponent->SetCurrentSlotIndex(Index);
    // 퀵슬롯에서 해당 인덱스에 있는 아이템 가져오기
    UNS_InventoryBaseItem* Item = QuickSlotComponent->GetItemInSlot(Index);

    // 아이템이 없으면 → 무기 해제 + 퀵슬롯 정리
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
        {
            if (WeaponComp->GetCurrentWeaponItem())
            {
                WeaponComp->UnequipWeapon();
                UE_LOG(LogTemp, Warning, TEXT("슬롯 비어 있음 - 무기 해제 (슬롯: %d)"), Index);
            }
        }

        Client_NotifyInventoryUpdated();
        return;
    }

    // 아이템이 존재하면 → 인벤토리에서 해당 인스턴스 직접 수량 감소
    PlayerInventory->RemoveAmountOfItem(Item, 1);

    // 감소 후 수량 확인 → 무기 해제
    if (Item->Quantity <= 0)
    {
        if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
        {
            if (WeaponComp->GetCurrentWeaponItem())
            {
                WeaponComp->UnequipWeapon();
                UE_LOG(LogTemp, Warning, TEXT("아이템 소진으로 무기 해제됨 (슬롯: %d)"), Index);
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("HandleUseThrowableItem 실행됨 - NetMode: %d"), GetNetMode());
    Client_NotifyInventoryUpdated();
}

// 서버에서 슬롯 할당 처리
void ANS_PlayerCharacterBase::Server_AssignQuickSlot_Implementation(int32 SlotIndex, UNS_InventoryBaseItem* Item)
{
    if (QuickSlotComponent)
    {
        QuickSlotComponent->AssignToSlot(SlotIndex, Item);
    }
}
// ==========================================================================================================================================
// 키 입력에 따른 퀵슬롯 선택 1 ~ 5번 퀵슬롯 버튼
void ANS_PlayerCharacterBase::HandleQuickSlotKeyInput(int32 KeyNumber)
{
    // =====================================================================
    // 1. 키 번호 변환 및 기본 검증
    // =====================================================================
    
    // 키보드 입력(1~5)을 배열 인덱스(0~4)로 변환
    // 예: 키보드 '1'키 → 배열 인덱스 0, 키보드 '2'키 → 배열 인덱스 1
    int32 SlotIndex = KeyNumber - 1;
    
    // 퀵슬롯 컴포넌트가 없으면 작업 불가능
    if (!QuickSlotComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleQuickSlotKeyInput: 퀵슬롯 컴포넌트가 없습니다."));
        return;
    }
    
    // =====================================================================
    // 2. 중복 선택 검사 (최적화)
    // =====================================================================
    
    // 이미 같은 슬롯이 선택되어 있고 무기가 장착된 경우 → 중복 작업 방지
    if (SlotIndex == CurrentQuickSlotIndex)
    {
        UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>();
        if (WeaponComp && WeaponComp->GetCurrentWeaponItem())
        {
            // 이미 같은 슬롯의 무기가 장착되어 있으면 아무 작업도 하지 않음
            UE_LOG(LogTemp, Verbose, TEXT("HandleQuickSlotKeyInput: 이미 슬롯 %d의 무기가 장착되어 있습니다."), SlotIndex + 1);
            return;
        }
    }
    
    // =====================================================================
    // 3. 현재 선택된 슬롯 업데이트
    // =====================================================================
    
    // 현재 슬롯 인덱스 업데이트 (클래스 멤버 변수)
    CurrentQuickSlotIndex = SlotIndex;
    
    // 퀵슬롯 컴포넌트의 현재 슬롯 인덱스도 함께 업데이트
    QuickSlotComponent->SetCurrentSlotIndex(CurrentQuickSlotIndex);
    UE_LOG(LogTemp, Verbose, TEXT("HandleQuickSlotKeyInput: 슬롯 %d 선택됨"), CurrentQuickSlotIndex + 1);
    
    // =====================================================================
    // 4. 네트워크 동기화 (서버/클라이언트)
    // =====================================================================
    
    // 클라이언트에서 실행 중이면 서버에 요청
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Verbose, TEXT("HandleQuickSlotKeyInput: 클라이언트에서 서버에 슬롯 %d 사용 요청"), CurrentQuickSlotIndex + 1);
        Server_UseQuickSlotByIndex(CurrentQuickSlotIndex);
    }
    else
    {
        // 서버에서 실행 중이면 직접 멀티캐스트 호출 (모든 클라이언트에 전파)
        UE_LOG(LogTemp, Verbose, TEXT("HandleQuickSlotKeyInput: 서버에서 슬롯 %d 사용 멀티캐스트"), CurrentQuickSlotIndex + 1);
        Multicast_UseQuickSlotByIndex(CurrentQuickSlotIndex);
    }
}

// 서버에 퀵슬롯 사용 요청
void ANS_PlayerCharacterBase::Server_UseQuickSlotByIndex_Implementation(int32 Index)
{
    // 인덱스가 음수면 0으로 설정
    if (Index < 0)
    {
        Index = 0;
    }
    
    // 현재 슬롯 인덱스 업데이트
    CurrentQuickSlotIndex = Index;
    
    // 서버에서 모든 클라이언트에 알림
    Multicast_UseQuickSlotByIndex(CurrentQuickSlotIndex);
}

// 모든 클라이언트에 퀵슬롯 사용 알림
void ANS_PlayerCharacterBase::Multicast_UseQuickSlotByIndex_Implementation(int32 Index)
{
    if (!QuickSlotComponent) 
    {
        return;
    }
    
    // 인덱스가 음수면 0으로 설정
    if (Index < 0)
    {
        Index = 0;
    }

    // 현재 슬롯 인덱스 업데이트
    CurrentQuickSlotIndex = Index;
    
    // 퀵슬롯 컴포넌트의 현재 슬롯 인덱스도 설정
    QuickSlotComponent->SetCurrentSlotIndex(CurrentQuickSlotIndex);
    
    // 슬롯에서 아이템 가져오기
    UNS_InventoryBaseItem* Item = QuickSlotComponent->GetItemInSlot(CurrentQuickSlotIndex);

    // 무기 컴포넌트 확인
    UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>();
    if (!WeaponComp)
    {
        return;
    }
    
    // 현재 장착된 무기 확인
    UNS_InventoryBaseItem* CurrentWeapon = WeaponComp->GetCurrentWeaponItem();
    
    // 이미 같은 무기가 장착되어 있으면 무시
    if (CurrentWeapon && Item && CurrentWeapon == Item)
    {
        return;
    }

    // 비어 있는 슬롯일 경우 → 현재 무기 해제
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        if (WeaponComp->GetCurrentWeaponItem())
        {
            // 무기 해제 처리
            WeaponComp->UnequipWeapon();
        }
        return;
    }

    // 아이템 데이터 확인
    const FNS_ItemDataStruct* ItemData = Item->GetItemData();
    if (!ItemData) 
    {
        return;
    }
    
    if (ItemData->ItemType != EItemType::Equipment) 
    {
        return;
    }

    // 무기 장착 처리 - 여기서 직접 장착하지 않고 애니메이션 플래그만 설정
    if (!ItemData->WeaponActorClass)
    {
        return;
    }
    
    // 무기 교체 애니메이션 플래그 설정 - 실제 무기 교체는 애니메이션 노티파이에서 처리
    IsChangeAnim = true;
    
    UE_LOG(LogTemp, Warning, TEXT("Multicast_UseQuickSlotByIndex: 무기 교체 애니메이션 시작 (IsChangeAnim = true), 슬롯: %d"), CurrentQuickSlotIndex + 1);
    
    // 여기서 WeaponComp->SwapWeapon(ItemData->WeaponActorClass, Item); 호출 제거
    // 실제 무기 교체는 NS_AN_EquipQuickSlotItem 노티파이에서 처리
}

// 아이템 획득 시 자동으로 퀵슬롯에 할당하고 장착 애니메이션 실행
void ANS_PlayerCharacterBase::AutoEquipPickedUpItem(UNS_InventoryBaseItem* NewItem)
{
    // 아이템이 없거나 장비 아이템이 아니면 무시
    if (!NewItem || NewItem->ItemType != EItemType::Equipment || NewItem->WeaponType == EWeaponType::Ammo)
    {
        return;
    }
    
    // 퀵슬롯 컴포넌트 확인
    if (!QuickSlotComponent)
    {
        return;
    }
    
    // 이미 퀵슬롯에 할당되어 있는지 확인
    bool bAlreadyAssigned = QuickSlotComponent->IsItemAlreadyAssigned(NewItem);
    
    // 아직 할당되지 않았다면 첫 번째 빈 슬롯에 할당
    if (!bAlreadyAssigned)
    {
        // 첫 번째 빈 슬롯에 할당 시도
        bool bAssigned = QuickSlotComponent->AssignToFirstEmptySlot(NewItem);
        
        // 할당 실패 시 (모든 슬롯이 차있는 경우) 첫 번째 슬롯에 강제 할당
        if (!bAssigned)
        {
            QuickSlotComponent->AssignToSlot(0, NewItem);
            UE_LOG(LogTemp, Warning, TEXT("AutoEquipPickedUpItem: 모든 슬롯이 차있어 슬롯 1에 강제 할당: %s"), *NewItem->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AutoEquipPickedUpItem: 첫 빈 슬롯에 할당 성공: %s"), *NewItem->GetName());
        }
    }
    
    // 현재 슬롯 인덱스를 새 아이템이 있는 슬롯으로 설정
    int32 ItemSlotIndex = QuickSlotComponent->FindSlotIndexForItem(NewItem);
    if (ItemSlotIndex != -1)
    {
        // 현재 슬롯 인덱스 업데이트
        CurrentQuickSlotIndex = ItemSlotIndex;
        QuickSlotComponent->SetCurrentSlotIndex(CurrentQuickSlotIndex);
        
        // 장착 애니메이션 실행 플래그 설정
        IsChangeAnim = true;
        
        // 1.4초 후 애니메이션 플래그 리셋
        FTimerHandle ResetAnimTimerHandle;
        GetWorldTimerManager().SetTimer(
            ResetAnimTimerHandle,
            FTimerDelegate::CreateLambda([this]() { 
                IsChangeAnim = false;
                UE_LOG(LogTemp, Verbose, TEXT("AutoEquipPickedUpItem: 장착 애니메이션 플래그 리셋"));
            }),
            1.4f,
            false
        );
        
        UE_LOG(LogTemp, Warning, TEXT("AutoEquipPickedUpItem: 슬롯 %d에 아이템 장착 애니메이션 시작: %s"), 
            CurrentQuickSlotIndex + 1, *NewItem->GetName());
            
        // 서버/클라이언트 상태에 따라 적절한 함수 호출
        if (!HasAuthority())
        {
            Server_UseQuickSlotByIndex(CurrentQuickSlotIndex);
        }
        else
        {
            Multicast_UseQuickSlotByIndex(CurrentQuickSlotIndex);
        }
    }
}
// ===============================================================================================================================

void ANS_PlayerCharacterBase::Server_UseInventoryItem_Implementation(FName ItemRowName)
{
    for (UNS_InventoryBaseItem* Item : PlayerInventory->GetInventoryContents())
    {
        if (Item && Item->ItemDataRowName == ItemRowName)
        {
            Item->OnUseItem(this);

            // 아래 코드 제거 또는 주석 처리 - Pickup.cpp에서 처리하므로 중복 방지
            /*
            // 장비 아이템일 경우 퀵슬롯 자동 등록
            if (Item->ItemType == EItemType::Equipment &&
                Item->WeaponType != EWeaponType::Ammo &&
                QuickSlotComponent)
            {
                QuickSlotComponent->AssignToFirstEmptySlot(Item);
                UE_LOG(LogTemp, Warning, TEXT("[Server] 퀵슬롯 자동 등록 완료: %s"), *Item->GetName());
            }
            */
            return;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("[Server] RowName으로 아이템 찾기 실패: %s"), *ItemRowName.ToString());
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

                if (QuickSlotComponent)
                {
                    QuickSlotComponent->BroadcastSlotUpdate();
                }
            }), 0.05f, false);
    }
}

// 클라이언트면 서버로 클라이언트 자신에 Yaw값과 Pitch값을 서버로 전송
void ANS_PlayerCharacterBase::UpdateAim_Server_Implementation(float Yaw, float Pitch)
{
    // 서버에서 변수 업데이트
    CamYaw = Yaw;
    CamPitch = Pitch;
    
    // 모든 클라이언트에 전파
    UpdateAim_Multicast(Yaw, Pitch);
}

void ANS_PlayerCharacterBase::UpdateAim_Multicast_Implementation(float Yaw, float Pitch)
{
    // 애니메이션 업데이트를 위한 변수 설정
    CamYaw = Yaw;
    CamPitch = Pitch;
    
    // 손전등이 켜져 있다면 회전도 업데이트
    if (bFlashlightOnOff && FlashlightComponent)
    {
        // 카메라 회전 계산 (캐릭터 회전 + Yaw/Pitch 오프셋)
        FRotator ActorRotation = GetActorRotation();
        FRotator CameraRot = ActorRotation;
        CameraRot.Yaw += Yaw;
        CameraRot.Pitch = Pitch; // Pitch는 직접 설정 (상대적이지 않음)
        
        // 손전등 회전 업데이트
        FlashlightComponent->SetWorldRotation(CameraRot);
    }
}

void ANS_PlayerCharacterBase::ThrowBottle()
{
    if (!HasAuthority() || bHasThrown || !BottleClass) return;

    bHasThrown = true;

    // 소켓 위치 가져오기
    FVector SpawnLocation = GetMesh()->DoesSocketExist(ThrowSocketName)
        ? GetMesh()->GetSocketLocation(ThrowSocketName)
        : GetActorLocation();

    FRotator ControlRot = GetControlRotation(); // 카메라 바라보는곳으로
    FVector LaunchDir = ControlRot.Vector(); // Pitch값도 적용해서 상/하 적용

    // 병 액터 생성
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;

    ANS_ThrowActor* Bottle = GetWorld()->SpawnActor<ANS_ThrowActor>(
        BottleClass, SpawnLocation, ControlRot, Params);

    // 병 액터에 있는 병이 깨지는 함수 실행
    if (Bottle)
    {
        Bottle->LaunchInDirection(LaunchDir);
    }

    // 중복 방지 해제
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        bHasThrown = false;
    });
}

void ANS_PlayerCharacterBase::ToggleFlashlight()
{
    if (HasAuthority())
    {
        ToggleFlashlight_Multicast();
    }
    else
    {
        ToggleFlashlight_Server();
    }
}

void ANS_PlayerCharacterBase::ToggleFlashlight_Server_Implementation()
{
    ToggleFlashlight_Multicast();
}

void ANS_PlayerCharacterBase::ToggleFlashlight_Multicast_Implementation()
{
    bFlashlightOnOff = !bFlashlightOnOff;
    FlashlightComponent->SetVisibility(bFlashlightOnOff);
}

void ANS_PlayerCharacterBase::UpdateTurnInPlace(float DeltaTime)
{
    if (!Controller) return;
    
    // 회전 중이 아니면 종료
    if (!bIsTurningInPlace) return;
    
    // 현재 카메라와 캐릭터 간의 Yaw 차이 계산
    const FRotator ActorRot = GetActorRotation();
    const FRotator ControlRot = Controller->GetControlRotation();
    const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);
    const float CurrentYawDelta = DeltaRot.Yaw;
    
    // 회전 방향 결정 왼쪽 또는 오른쪽
    const float RotationDirection = TurnLeft ? -1.0f : 1.0f;
    
    // 캐릭터 회전을 부드럽게 회전하도록 설정
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += RotationDirection * TurnInPlaceSpeed * DeltaTime;
    SetActorRotation(NewRotation);
    
    // CamYaw 값을 부드럽게 보간 애님인스턴스인 ABP에서 사용할 값임
    // 회전 중에는 CamYaw를 현재 회전 방향에 맞게 부드럽게 감소
    float TargetYaw = 0.0f;
    if (TurnLeft)
    {
        // 왼쪽 회전 중이면 음수 값에서 0으로 보간
        TargetYaw = FMath::Min(0.0f, CurrentYawDelta);
    }
    else
    {
        // 오른쪽 회전 중이면 양수 값에서 0으로 보간
        TargetYaw = FMath::Max(0.0f, CurrentYawDelta);
    }
    
    CamYaw = FMath::FInterpTo(CamYaw, TargetYaw, DeltaTime, TurnInPlaceSpeed * 0.5f);
    
    // 회전이 충분히 이루어졌는지 확인 임계값 이하로 떨어졌는지
    if (FMath::Abs(CurrentYawDelta) <= TurnInPlaceResetThreshold)
    {
        // 회전 종료 노티파이에서 OnTurnInPlaceFinished 호출될거임
        bIsTurningInPlace = false;
        
        // 마지막 CamYaw 값 저장 ===== 부드러운 리셋을 위해
        LastTurnYaw = CamYaw;
        bIsResettingYaw = true;
    }
    
    // 서버에 업데이트된 CamYaw 전송
    UpdateAim_Server(CamYaw, CamPitch);
}

// 애니메이션 노티파이에서 호출할 함수 수정
void ANS_PlayerCharacterBase::OnTurnInPlaceFinished()
{
    // 회전 완료 후 변수 초기화
    bIsTurningInPlace = false;
    
    // 마지막 CamYaw 값 저장 (부드러운 리셋을 위해)
    if (!bIsResettingYaw)
    {
        LastTurnYaw = CamYaw;
        bIsResettingYaw = true;
    }
    
    // bUseControllerDesiredRotation 비활성화
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    
    // 서버에 상태 업데이트
    if (HasAuthority())
    {
        // 서버에서 직접 설정하고 멀티캐스트
        TurnLeft = false;
        TurnRight = false;
        Multicast_UpdateTurnInPlaceState(false, false, false);
    }
    else
    {
        // 클라이언트에서는 서버에 요청
        Server_UpdateTurnInPlaceState(false, false, false);
    }
}

void ANS_PlayerCharacterBase::Server_UpdateTurnInPlaceState_Implementation(bool bInTurnLeft, bool bInTurnRight, bool bInUseControllerDesiredRotation)
{
    // 서버에서 상태 업데이트
    TurnLeft = bInTurnLeft;
    TurnRight = bInTurnRight;
    GetCharacterMovement()->bUseControllerDesiredRotation = bInUseControllerDesiredRotation;
    
    // 모든 클라이언트에 멀티캐스트
    Multicast_UpdateTurnInPlaceState(bInTurnLeft, bInTurnRight, bInUseControllerDesiredRotation);
}

void ANS_PlayerCharacterBase::Multicast_UpdateTurnInPlaceState_Implementation(bool bInTurnLeft, bool bInTurnRight, bool bInUseControllerDesiredRotation)
{
    // 로컬 플레이어가 아닌 경우에만 적용
    if (!IsLocallyControlled())
    {
        TurnLeft = bInTurnLeft;
        TurnRight = bInTurnRight;
        GetCharacterMovement()->bUseControllerDesiredRotation = bInUseControllerDesiredRotation;
    }
}

void ANS_PlayerCharacterBase::UpdateYawReset(float DeltaTime)
{
    if (!bIsResettingYaw) return;
    
    // CamYaw를 부드럽게 0으로 보간
    CamYaw = FMath::FInterpTo(CamYaw, 0.0f, DeltaTime, TurnInPlaceYawResetSpeed);
    
    // 충분히 0에 가까워지면 리셋 완료
    if (FMath::Abs(CamYaw) < 0.5f)
    {
        CamYaw = 0.0f;
        bIsResettingYaw = false;
    }
    
    // 서버에 업데이트된 CamYaw 전송
    UpdateAim_Server(CamYaw, CamPitch);
}

