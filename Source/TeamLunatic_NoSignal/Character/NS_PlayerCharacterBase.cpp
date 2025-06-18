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
#include "Inventory UI/Inventory/NS_QuickSlotPanel.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineMeshComponent.h"
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

    // 퀵슬롯 
    QuickSlotComponent = CreateDefaultSubobject<UNS_QuickSlotComponent>(TEXT("QuickSlotComponent"));
    QuickSlotComponent->SetIsReplicated(true);
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
             ETriggerEvent::Started,
              this,
               &ANS_PlayerCharacterBase::ReloadAction_Server);
        }

        if (InputQuickSlot1)
        {
            EnhancedInput->BindAction(
                InputQuickSlot1, 
                ETriggerEvent::Started, 
                this, 
                &ANS_PlayerCharacterBase::UseQuickSlot1);
        }
        if (InputQuickSlot2)
        {
            EnhancedInput->BindAction(
                InputQuickSlot2,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::UseQuickSlot2);
        }       
        if (InputQuickSlot3)
        {
            EnhancedInput->BindAction(
                InputQuickSlot3,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::UseQuickSlot3);
        }
    }
}

void ANS_PlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANS_PlayerCharacterBase, IsSprint);  // 달리기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsPickUp);  // 아이템줍기 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsHit);     // 맞는지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamYaw);    // 카메라 좌/우 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamPitch);  // 카메라 상/하 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAiming);  // 조준중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsReload); // 장전중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnLeft);  // 몸을 왼쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnRight); // 몸을 오른쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, NowFire);   // 사격시 몸전체Mesh 사격 애니메이션 재생 용 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, PlayerInventory);
    DOREPLIFETIME(ANS_PlayerCharacterBase, QuickSlotComponent);
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsChangeAnim); // 퀵슬롯 눌렀을때 무기 장착하는 애니메이션 재생 용 변수
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
    AddControllerYawInput  (LookInput.X * LookMagnification); 
	UE_LOG(LogTemp, Warning, TEXT("LookInput.X: %f"), LookInput.X); // Yaw값 확인용 로그
    AddControllerPitchInput(LookInput.Y * LookMagnification); 

    // Actor Rotation과 Control Rotation을 Delta를 이용해 Yaw값 추출
    const FRotator ActorRot   = GetActorRotation(); 
    const FRotator ControlRot = Controller->GetControlRotation(); 
    const FRotator DeltaRot   = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);
    
    const float DeltaTime = GetWorld()->GetDeltaSeconds();
    
    //CamYaw   = FMath::FInterpTo(CamYaw,   DeltaRot.Yaw,   DeltaTime, AimSendInterpSpeed);
	CamYaw = DeltaRot.Yaw; // 서버로 전송할 때는 보간하지 않고 바로 전송
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
    if (GetCharacterMovement()->IsFalling()) { return; } 
    
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

void ANS_PlayerCharacterBase::PickUpAction_Server_Implementation(const FInputActionValue& Value)
{
    if (GetCharacterMovement()->IsFalling()) {return;} 

    IsPickUp = true; 
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
    // 실제 총알 재장전 로직은 애님노티파이로 애니메이션 안에서 EquipedWeapon에있는 Server_Reload()함수를 블루프린트로 실행 할 예정
    
    // 현재 무기가 없거나, 원거리 무기가 아니면 재장전 불가
    if (!EquipedWeaponComp->CurrentWeapon)// 현재 무기가 없으면 return
    {
        return;
    }

    // 근거리 무기면 return
    if (EquipedWeaponComp->CurrentWeapon->GetWeaponType() == EWeaponType::Melee)
    {
        return;
    }
    
	IsReload = true;

    // 노티파이로 IsReload 변수값을 false로 변경하고 있음
}

//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////


void ANS_PlayerCharacterBase::PlayDeath_Server_Implementation()
{
    if (UWorld* World = GetWorld())
    {
        ANS_GameModeBase* BaseGameMode = Cast<ANS_GameModeBase>(UGameplayStatics::GetGameMode(World));
        if (BaseGameMode)
        {
            BaseGameMode->OnPlayerCharacterDied(this); 
        }
    }

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
    if (!HasAuthority())
    {
        Server_UseThrowableItem(Index); 
    }
}

void ANS_PlayerCharacterBase::Server_UseThrowableItem_Implementation(int32 Index)
{
    HandleUseThrowableItem(Index);
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

void ANS_PlayerCharacterBase::UseQuickSlot1() { UseQuickSlotByIndex(0); }
void ANS_PlayerCharacterBase::UseQuickSlot2() { UseQuickSlotByIndex(1); }
void ANS_PlayerCharacterBase::UseQuickSlot3() { UseQuickSlotByIndex(2); }

void ANS_PlayerCharacterBase::UseQuickSlotByIndex(int32 Index)
{
    if (HasAuthority())
    {
        Multicast_UseQuickSlotByIndex(Index);
    }
    else
    {
        Server_UseQuickSlotByIndex(Index);  // 클라 → 서버 요청
    }
}

void ANS_PlayerCharacterBase::Server_UseQuickSlotByIndex_Implementation(int32 Index)
{
    if (!QuickSlotComponent) return;
    QuickSlotComponent->SetCurrentSlotIndex(Index);
    UNS_InventoryBaseItem* Item = QuickSlotComponent->GetItemInSlot(Index);
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        // 무기 장착 중이면 장착 해제 처리
        if (EquipedWeaponComp && EquipedWeaponComp->GetCurrentWeaponItem())
        {
            if (!IsChangeAnim)
            {
                IsChangeAnim = true;  // 애니메이션 실행 상태 플래그
                UE_LOG(LogTemp, Warning, TEXT("[Server_UseQuickSlot] 슬롯 %d 애니메이션 시작 준비"), Index);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[Server_UseQuickSlot] 슬롯 %d 비어 있음 - 장착 무기 없음"), Index);
        }

        return;  // 무기 없으므로 더 이상 진행하지 않음
    }

    if (!IsChangeAnim)
    {
        IsChangeAnim = true;  // 애니메이션 실행 상태 플래그
        UE_LOG(LogTemp, Warning, TEXT("[Server_UseQuickSlot] 슬롯 %d 애니메이션 시작 준비"), Index);
    }
}

void ANS_PlayerCharacterBase::Multicast_UseQuickSlotByIndex_Implementation(int32 Index)
{
    UseQuickSlotByIndex_Internal(Index);
}

void ANS_PlayerCharacterBase::UseQuickSlotByIndex_Internal(int32 Index)
{
    if (!QuickSlotComponent) return;

    UNS_InventoryBaseItem* Item = QuickSlotComponent->GetItemInSlot(Index);

    // 비어 있는 슬롯일 경우 → 현재 무기 해제
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
        {
            if (WeaponComp->GetCurrentWeaponItem())
            {
                WeaponComp->UnequipWeapon();
                UE_LOG(LogTemp, Warning, TEXT("[UseQuickSlot_Internal] 빈 슬롯 선택 - 무기 해제 완료 (슬롯: %d)"), Index);
            }
        }

        QuickSlotComponent->SetCurrentSlotIndex(Index);
        return;
    }
    if (!Item || Item->ItemDataRowName.IsNone()) return;

    // 정상 아이템일 경우
    const FNS_ItemDataStruct* ItemData = Item->GetItemData();
    if (!ItemData || ItemData->ItemType != EItemType::Equipment) return;

    if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
    {
        WeaponComp->SwapWeapon(ItemData->WeaponActorClass, Item);
    }

    QuickSlotComponent->SetCurrentSlotIndex(Index);
    UE_LOG(LogTemp, Warning, TEXT("[UseQuickSlot_Internal] 장비 장착 - 인덱스: %d, 아이템: %s"), Index, *Item->GetName());
}

void ANS_PlayerCharacterBase::OnRep_IsChangeAnim()
{
    if (IsChangeAnim)
    {
        UE_LOG(LogTemp, Warning, TEXT("클라이언트에서 IsChangeAnim TRUE 감지됨"));
    }
}

void ANS_PlayerCharacterBase::Server_UseInventoryItem_Implementation(FName ItemRowName)
{
    for (UNS_InventoryBaseItem* Item : PlayerInventory->GetInventoryContents())
    {
        if (Item && Item->ItemDataRowName == ItemRowName)
        {
            Item->OnUseItem(this);

            // 장비 아이템일 경우 퀵슬롯 자동 등록
            if (Item->ItemType == EItemType::Equipment &&
                Item->WeaponType != EWeaponType::Ammo &&
                QuickSlotComponent)
            {
                QuickSlotComponent->AssignToFirstEmptySlot(Item);
                UE_LOG(LogTemp, Warning, TEXT("[Server] 퀵슬롯 자동 등록 완료: %s"), *Item->GetName());
            }
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
void ANS_PlayerCharacterBase::UpdateAim_Server_Implementation(float NewCamYaw, float NewCamPitch)
{
    CamYaw   = NewCamYaw; 
    CamPitch = NewCamPitch; 
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