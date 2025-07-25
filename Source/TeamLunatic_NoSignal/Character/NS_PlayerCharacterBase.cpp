#include "Character/NS_PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.H"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Components/NS_InventoryComponent.h"
#include "Item/NS_InventoryBaseItem.h"
#include "Character/NS_PlayerController.h"
#include "Components/NS_EquipedWeaponComponent.h"
#include "Character/Components/NS_StatusComponent.h"
#include "UI/InGame/NS_InventoryMainWidget.h"
#include "Item/NS_BaseRangedWeapon.h"
#include "Character/ThrowActor/NS_ThrowActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Character/Components/NS_InteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/Pickup.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include <Net/UnrealNetwork.h>
#include "Character/Components//NS_QuickSlotComponent.h"
#include "Item/NS_BaseWeapon.h"
#include "UI/InGame/NS_LevelMapWidget.h"
#include "Sound/SoundBase.h"
#include "UI/HUD/NS_InGameHUD.h"
#include "UI/InGame/NS_PlayerWidget.h"
#include "Character/DeathBox/NS_DeathBox.h"

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
    InteractionComp = CreateDefaultSubobject<UNS_InteractionComponent>(TEXT("InteractionComponent"));
    // 장착 무기 컴포넌트
    EquipedWeaponComp = CreateDefaultSubobject<UNS_EquipedWeaponComponent>(TEXT("EquipedWeaponComponent"));

    // 인벤토리
    InventoryComp = CreateDefaultSubobject<UNS_InventoryComponent>(TEXT("PlayerInventory"));
    SetReplicates(true);
    InventoryComp->SetSlotsCapacity(20);
    InventoryComp->SetWeightCapacity(50.0f);

    // 퀵슬롯 
    QuickSlotComp = CreateDefaultSubobject<UNS_QuickSlotComponent>(TEXT("QuickSlotComponent"));
    QuickSlotComp->SetIsReplicated(true);

    // 스팟라이트 컴포넌트 
    FlashlightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightComp"));
    FlashlightComp->SetupAttachment(CameraComp);  // 카메라에 부착
    FlashlightComp->SetRelativeRotation(FRotator::ZeroRotator);
    FlashlightComp->SetVisibility(true); 
    FlashlightComp->SetIntensity(8000.0f);
    FlashlightComp->SetOuterConeAngle(30.0f);
    FlashlightComp->SetInnerConeAngle(15.0f);
    FlashlightComp->SetAttenuationRadius(2000.0f);
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
            // 기본 매핑 컨텍스트 추가 (우선순위 0)
            Sub->AddMappingContext(DefaultMappingContext, 0);
            
            // UI 매핑 컨텍스트가 있으면 추가하지만 기본적으로 비활성화 상태
            // UI 모드에서만 활성화되도록 우선순위를 높게 설정하지 않음
            if (UIMappingContext)
            {
                // UI 매핑 컨텍스트는 추가하지 않고 필요할 때만 추가함
            }
        }
    }

    // 기본 속도 설정
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
    }
    
    // 기본 퀵슬롯는 1번부터 시작되도록 
    CurrentQuickSlotIndex = 0;
    if (QuickSlotComp)
    {
        QuickSlotComp->SetCurrentSlotIndex(CurrentQuickSlotIndex);
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
        // 캐릭터 이동
        if (InputMoveAction)
        {
            EnhancedInput->BindAction(
                InputMoveAction,
                ETriggerEvent::Triggered,
                this,
                &ANS_PlayerCharacterBase::MoveAction);
        }
        
        // 캐릭터 회전
        if (InputLookAction)
        {
            EnhancedInput->BindAction(
                InputLookAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::LookAction);
        }

        // 점프
        if (InputJumpAction)
        {
            EnhancedInput->BindAction(
                InputJumpAction,
             ETriggerEvent::Triggered,
              this,
               &ANS_PlayerCharacterBase::JumpAction);
        }

        // 앉기
        if (InputCrouchAction)
        {
            EnhancedInput->BindAction(
            InputCrouchAction,
             ETriggerEvent::Started,
              this,
               &ANS_PlayerCharacterBase::StartCrouch);
            EnhancedInput->BindAction(
            InputCrouchAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopCrouch);
        }

        // 달리기
        if (InputSprintAction)
        {
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Started,
              this,
               &ANS_PlayerCharacterBase::StartSprint);
            EnhancedInput->BindAction(
            InputSprintAction,
             ETriggerEvent::Completed,
              this,
               &ANS_PlayerCharacterBase::StopSprint);
        }

        // 상호작용
        if (InteractAction)
        {
            EnhancedInput->BindAction(
                InteractAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::StartInteraction_Server
            );

             EnhancedInput->BindAction(
                 InteractAction,
                 ETriggerEvent::Completed,
                 InteractionComp,
                 &UNS_InteractionComponent::EndInteract
             );
        }

        // 헤드라이트 On/Off
        if (InputFlashlightAction)
        {
            EnhancedInput->BindAction(
                InputFlashlightAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::ToggleFlashlight
                );
        }

        // 인벤토리 위젯 열고/닫기
        if (ToggleInventoryWidgetAction)
        {
            EnhancedInput->BindAction(
                ToggleInventoryWidgetAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::ToggleInventoryWidget
            );
        }

        // 레벨지도 위젯 열고/닫기
        if (ToggleOpenMapWidgetAction)
        {
            EnhancedInput->BindAction(
                ToggleOpenMapWidgetAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::ToggleOpenMapWidget
                );
        }

        // ESC키로 설정 위젯 열고/닫기
        if (ToggleESCWidgetAction)
        {
            EnhancedInput->BindAction(
                ToggleESCWidgetAction,
                ETriggerEvent::Started,
                this,
                &ANS_PlayerCharacterBase::ToggleESCWidget
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
    DOREPLIFETIME(ANS_PlayerCharacterBase, LookMagnification);
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamYaw);              // 카메라 좌/우 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, CamPitch);            // 카메라 상/하 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsAiming);            // 조준중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsReload);            // 장전중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnLeft);            // 몸을 왼쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, TurnRight);           // 몸을 오른쪽으로 회전시키는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, NowFire);             // 사격시 몸전체Mesh 사격 애니메이션 재생 용 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, bFlashlightOnOff);    // 헤드램프 키고 끄는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, InventoryComp);
    DOREPLIFETIME(ANS_PlayerCharacterBase, QuickSlotComp);
    DOREPLIFETIME(ANS_PlayerCharacterBase, StatusComp);          // 상태 컴포넌트 복제
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsChangeAnim);        // 퀵슬롯 눌렀을때 무기 장착하는 애니메이션 재생 용 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsDead);	             // 캐릭터가 죽었는지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsChangingWeapon);    // 무기 교체중인지 확인 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, bHasThrown);          // 한번만 던져지도록 실행하는 변수
    DOREPLIFETIME(ANS_PlayerCharacterBase, IsThrow);             // 병 던질 수 있는지 확인 변수
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
                    // 기존 매핑 컨텍스트를 완전히 제거
                    Subsystem->RemoveMappingContext(DefaultMappingContext);
                    
                    // UI 매핑 컨텍스트가 있으면 높은 우선순위로 추가
                    if (UIMappingContext)
                    {
                        Subsystem->AddMappingContext(UIMappingContext, 0); // 높은 우선순위로 추가
                    }
                    
                    // 마우스 커서 표시 및 UI 모드 설정
                    PC->SetShowMouseCursor(true);
                    PC->SetInputMode(FInputModeGameAndUI());
                }
                else
                {
                    // 게임 모드일 때는 UI 매핑 컨텍스트를 제거하고 기본 매핑 컨텍스트의 우선순위를 높임
                    if (UIMappingContext)
                    {
                        Subsystem->RemoveMappingContext(UIMappingContext);
                    }
                    
                    // 기존 매핑 컨텍스트를 제거했다가 다시 기본 우선순위로 추가
                    Subsystem->RemoveMappingContext(DefaultMappingContext);
                    Subsystem->AddMappingContext(DefaultMappingContext, 0); // 기본 우선순위로 추가
                    
                    PC->SetShowMouseCursor(false);
                    PC->SetInputMode(FInputModeGameOnly());
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

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // 서버에서만 실제 데미지 처리 및 멀티캐스트 전송
    if (HasAuthority() && ActualDamage > 0.f)
    {
        // 캐릭터 체력 감소
        StatusComp->UpdateHealthChange(-ActualDamage);
    }

    Multicast_HandleDamageEffects();

    return ActualDamage;
}


void ANS_PlayerCharacterBase::Multicast_HandleDamageEffects_Implementation()
{
    // 데미지 사운드 재생
    if (DamageSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DamageSound, GetActorLocation());
    }

    // 로컬 플레이어 컨트롤러에만 히트 이펙트 표시
    if (IsLocallyControlled())
    {
       ActivateHallucinationEffect();
    }

    // 피격 애니메이션을 위한 IsHit 플래그 설정 및 리셋 타이머
    IsHit = true;
    FTimerHandle ResetHitTimer;
    GetWorldTimerManager().SetTimer(ResetHitTimer, [this]() {
        if (IsValid(this))
        { 
            IsHit = false;
        }
    }, 0.5f, false);
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
        if (FMath::Abs(CamYaw) >= TurnInPlaceThreshold)
        {
            // 서버에 회전 시작을 요청합니다.
            Server_StartTurn(CamYaw < 0, CamYaw > 0);

            // 클라이언트에서도 즉시 상태를 업데이트하여 부드러운 전환을 만듭니다.
            bIsTurningInPlace = true;
            bIsResettingYaw = false;
            CurrentTurnYaw = CamYaw;
            LastTurnYaw = CamYaw;
            GetCharacterMovement()->bUseControllerDesiredRotation = true;
        }
    }
    
    CamPitch = FMath::FInterpTo(CamPitch, DeltaRot.Pitch, DeltaTime, AimSendInterpSpeed); 

    // 조준 정보 전송 요청
    RequestUpdateAim();
}

void ANS_PlayerCharacterBase::JumpAction(const FInputActionValue& Value)
{
    bool IsJump = Value.Get<bool>(); 
    
    if (IsJump && IsCanJump) 
    {
        Jump(); 
        IsCanJump = false; 

        // 기존 타이머가 있다면 클리어
        if (GetWorldTimerManager().IsTimerActive(JumpTimerHandle))
        {
            GetWorldTimerManager().ClearTimer(JumpTimerHandle);
        }

        // 점프한 뒤로 1.3초동안은 점프를 못함
        GetWorldTimerManager().SetTimer( 
            JumpTimerHandle, 
            FTimerDelegate::CreateLambda([this]() { 
                if (IsValid(this))
                {
                    IsCanJump = true; 
                }
            }), 
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

void ANS_PlayerCharacterBase::StartSprint(const FInputActionValue& Value)
{
       Server_SetSprinting(true);
}

void ANS_PlayerCharacterBase::StopSprint(const FInputActionValue& Value)
{
       Server_SetSprinting(false);
}

void ANS_PlayerCharacterBase::StartInteraction_Server_Implementation(const FInputActionValue& Value)
{
    // 상호작용이 불가능한 상태인지 확인
    if (GetCharacterMovement()->IsFalling() || // 낙하 중
        IsPickUp ||                           // 이미 아이템 줍기 중
        IsReload ||                           // 재장전 중
        IsChangeAnim ||                       // 무기 교체 애니메이션 중
        (EquipedWeaponComp && EquipedWeaponComp->IsAttack) || // 공격 중
        IsThrow)                              // 투척 중
    {
        return;
    }

    // 상호작용 컴포넌트를 통해 상호작용 시작
    if (UNS_InteractionComponent* InteractComp = FindComponentByClass<UNS_InteractionComponent>())
    {
        InteractComp->BeginInteract();
    }
}

void ANS_PlayerCharacterBase::ToggleOpenMapWidget()
{
    if (!IsLocallyControlled()) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!IsValid(PC)) return;

    ANS_InGameHUD* InGameHUD = Cast<ANS_InGameHUD>(PC->GetHUD());
    if (!IsValid(InGameHUD)) return;

    UNS_LevelMapWidget* OpenLevelWidget = InGameHUD->GetLevelMapWidget();
    if (!IsValid(OpenLevelWidget)) return;

    UNS_PlayerWidget* PlayerWidget = InGameHUD->GetPlayerWidget();
    if (!IsValid(PlayerWidget)) return;

    UNS_InventoryMainWidget* InventoryWidget = InGameHUD->GetInventoryMainWidget();
    if (!IsValid(PlayerWidget)) return;

    UUserWidget* ESCWidget = InGameHUD->GetESCWidget();
    if (!IsValid(ESCWidget)) return;

    // ESC 메뉴가 열려 있을 때도 맵 키가 작동하도록 함
    if (InGameHUD->GetCurrentWidget() == ESCWidget)
    {
        // ESC 메뉴 닫고 맵 열기
        InGameHUD->ShowWidget(OpenLevelWidget);
        
        // 맵 열릴 때 이동 기능 비활성화
        SetMovementLockState_Server(true);
    }
    else if (InGameHUD->GetCurrentWidget() == OpenLevelWidget)
    {
        // 맵이 열려 있으면 닫기
        InGameHUD->ShowWidget(PlayerWidget);
        
        // 맵 닫을 때 이동 기능 활성화
        SetMovementLockState_Server(false);
    }
    else
    {
        // 다른 위젯이 열려 있으면 맵 열기
        InGameHUD->ShowWidget(OpenLevelWidget);
        
        // 맵 열릴 때 이동 기능 비활성화
        SetMovementLockState_Server(true);
    }
}

void ANS_PlayerCharacterBase::ToggleInventoryWidget()
{
    if (!IsLocallyControlled()) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!IsValid(PC)) return;

    ANS_InGameHUD* InGameHUD = Cast<ANS_InGameHUD>(PC->GetHUD());
    if (!IsValid(InGameHUD)) return;

    UNS_InventoryMainWidget* InventoryMainWidget = InGameHUD->GetInventoryMainWidget();
    if (!IsValid(InventoryMainWidget)) return;

    UNS_PlayerWidget* PlayerWidget = InGameHUD->GetPlayerWidget();
    if (!IsValid(PlayerWidget)) return;

    UUserWidget* ESCWidget = InGameHUD->GetESCWidget();
    if (!IsValid((ESCWidget))) return;

    // ESC 메뉴가 열려 있을 때도 인벤토리 키가 작동하도록 함
    if (InGameHUD->GetCurrentWidget() == ESCWidget)
    {
        // ESC 메뉴 닫고 인벤토리 열기
        InGameHUD->ShowWidget(InventoryMainWidget);
        
        // 인벤토리 열릴 때 이동 기능 비활성화
        SetMovementLockState_Server(true);
    }
    else if (InGameHUD->GetCurrentWidget() == InventoryMainWidget)
    {
        // 인벤토리가 열려 있으면 닫기
        InGameHUD->ShowWidget(PlayerWidget);
        
        // 인벤토리 닫을 때 이동 기능 활성화
        SetMovementLockState_Server(false);
    }
    else
    {
        // 다른 위젯이 열려 있으면 인벤토리 열기
        InGameHUD->ShowWidget(InventoryMainWidget);
        
        // 인벤토리 열릴 때 이동 기능 비활성화
        SetMovementLockState_Server(true);
    }
}

void ANS_PlayerCharacterBase::ToggleESCWidget()
{
    if (!IsLocallyControlled()) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!IsValid(PC)) return;

    ANS_InGameHUD* InGameHUD = Cast<ANS_InGameHUD>(PC->GetHUD());
    if (!IsValid(InGameHUD)) return;

    UNS_PlayerWidget* PlayerWidget = InGameHUD->GetPlayerWidget();
    if (!IsValid(PlayerWidget)) return;

    UUserWidget* ESCWidget = InGameHUD->GetESCWidget();
    if (!IsValid(ESCWidget)) return;

    if (InGameHUD->GetCurrentWidget() == ESCWidget)
    {
        // ESC메뉴위젯이 열려 있으면 닫기
        InGameHUD->ShowWidget(PlayerWidget);
        
        // ESC 메뉴 닫을 때 이동 기능 활성화
        SetMovementLockState_Server(false);
    }
    else
    {
        // 다른 위젯이 열려 있으면 ESC메뉴 열기
        InGameHUD->ShowWidget(ESCWidget);
        
        // ESC 메뉴 열릴 때 이동 기능 비활성화
        SetMovementLockState_Server(true);
    }
}
//////////////////////////////////액션 처리 함수들 끝!///////////////////////////////////




void ANS_PlayerCharacterBase::PlayDeath_Server_Implementation()
{
    if (EquipedWeaponComp)
    {
        EquipedWeaponComp->UnequipWeapon();
    }

    // 죽을 때 데스 박스 생성
    CreateDeathBox();

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
    
    //// 컨트롤러에서 분리시키고
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
    if (InventoryComp->FindMatchingItem(ItemToDrop))
    {
        if (EquipedWeaponComp && EquipedWeaponComp->GetCurrentWeaponItem() == ItemToDrop)
        {
            EquipedWeaponComp->UnequipWeapon();
        }
        QuickSlotComp->RemoveItem(ItemToDrop);
        Client_NotifyQuickSlotUpdated();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.bNoFail = true;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        const FVector ForwardOffset = GetActorForwardVector() * 100.0f; // 플레이어 앞 100cm
        const FVector SpawnLocation = GetActorLocation() + ForwardOffset + FVector(0.f, 0.f, 50.f); // 약간 위로 올림
        const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

        const int32 RemovedQuantity = InventoryComp->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);
        if (RemovedQuantity <= 0)
        {
            return;
        }

        APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

        Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
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
    if (QuickSlotComp)
    {
        QuickSlotComp->BroadcastSlotUpdate(); // UI 갱신
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
    }
}

void ANS_PlayerCharacterBase::HandleUseThrowableItem(int32 Index)
{
    if (!QuickSlotComp || !InventoryComp) return;
    QuickSlotComp->SetCurrentSlotIndex(Index);
    // 퀵슬롯에서 해당 인덱스에 있는 아이템 가져오기
    UNS_InventoryBaseItem* Item = QuickSlotComp->GetItemInSlot(Index);

    // 아이템이 없으면 → 무기 해제 + 퀵슬롯 정리
    if (!Item || Item->ItemDataRowName.IsNone())
    {
        if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
        {
            if (WeaponComp->GetCurrentWeaponItem())
            {
                WeaponComp->UnequipWeapon();
            }
        }

        Client_NotifyInventoryUpdated();
        return;
    }

    // 아이템이 존재하면 → 인벤토리에서 해당 인스턴스 직접 수량 감소
    InventoryComp->RemoveAmountOfItem(Item, 1);

    // 감소 후 수량 확인 → 무기 해제
    if (Item->Quantity <= 0)
    {
        if (UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>())
        {
            if (WeaponComp->GetCurrentWeaponItem())
            {
                WeaponComp->UnequipWeapon();
            }
        }
    }
    Client_NotifyInventoryUpdated();
}

// 서버에서 슬롯 할당 처리
void ANS_PlayerCharacterBase::Server_AssignQuickSlot_Implementation(int32 SlotIndex, UNS_InventoryBaseItem* Item)
{
    if (QuickSlotComp)
    {
        QuickSlotComp->AssignToSlot(SlotIndex, Item);
    }
}
// =======================================================퀵슬롯 함수 시작!================================================================
void ANS_PlayerCharacterBase::HandleQuickSlotKeyInput(int32 KeyNumber)
{
    // 퀵슬롯 컴포넌트가 없으면 작업 불가능
    if (!QuickSlotComp)
    {
        return;
    }
    
    if (!CanHandleQuickSlotInput(KeyNumber))
    {
        return;
    }
    
    // 현재 선택된 슬롯대로 캐릭터에 현재 슬롯 인덱스 업데이트
    CurrentQuickSlotIndex = KeyNumber;
    
    // 퀵슬롯 컴포넌트의 현재 슬롯 인덱스도 함께 업데이트
    QuickSlotComp->SetCurrentSlotIndex(CurrentQuickSlotIndex);
    
    // 네트워크 동기화 (서버/클라이언트)
    // 클라이언트에서 실행 중이면 서버에 요청
    if (!HasAuthority())
    {
        Server_UseQuickSlotByIndex(CurrentQuickSlotIndex);
    }
    else
    {
        // 서버에서 실행 중이면 직접 멀티캐스트 호출 (모든 클라이언트에 전파)
        Multicast_UseQuickSlotByIndex(CurrentQuickSlotIndex);
    }
}

void ANS_PlayerCharacterBase::Server_UseQuickSlotByIndex_Implementation(int32 Index)
{
    // 인덱스가 음수면 0으로 설정
    if (Index < 0)
    {
        Index = 0;
    }
    
    // 현재 선택된 슬롯대로 캐릭터에 현재 슬롯 인덱스 업데이트
    CurrentQuickSlotIndex = Index;
    
    // 서버에서 모든 클라이언트에 알림
    Multicast_UseQuickSlotByIndex(CurrentQuickSlotIndex);
}

void ANS_PlayerCharacterBase::Multicast_UseQuickSlotByIndex_Implementation(int32 Index)
{
    if (!QuickSlotComp) 
    {
        return;
    }
    
    // 인덱스가 음수면 0으로 설정
    if (Index < 0)
    {
        Index = 0;
    }

    // 현재 선택된 슬롯대로 캐릭터에 현재 슬롯 인덱스 업데이트
    CurrentQuickSlotIndex = Index;
    
    // 퀵슬롯 컴포넌트의 현재 슬롯 인덱스도 설정
    QuickSlotComp->SetCurrentSlotIndex(CurrentQuickSlotIndex);
    
    // 슬롯에서 아이템 가져오기
    UNS_InventoryBaseItem* Item = QuickSlotComp->GetItemInSlot(CurrentQuickSlotIndex);

    // 무기 컴포넌트 확인
    UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>();
    if (!WeaponComp)
    {
        return;
    }
    
    // 현재 장착된 무기 확인
}

bool ANS_PlayerCharacterBase::CanHandleQuickSlotInput(int32 SlotIndex)
{
    // 퀵슬롯 컴포넌트가 없으면 작업 불가능
    if (!QuickSlotComp)
    {
        return false;
    }

    // 중복 선택 검사
    // 이미 같은 슬롯이 선택되어 있고 무기가 장착된 경우 → 중복 작업 방지
    if (SlotIndex == CurrentQuickSlotIndex)
    {
        UNS_EquipedWeaponComponent* WeaponComp = FindComponentByClass<UNS_EquipedWeaponComponent>();
        if (WeaponComp && WeaponComp->GetCurrentWeaponItem())
        {
            // 이미 같은 슬롯의 무기가 장착되어 있으면 아무 작업도 하지 않음
            return false;
        }
    }
    return true;
}

// 아이템 획득 시 자동으로 퀵슬롯에 할당하고 장착 애니메이션 실행
void ANS_PlayerCharacterBase::AutoEquipPickedUpItem(UNS_InventoryBaseItem* NewItem)
{
    // 아이템이 없거나 장비 아이템이 아니면 리턴
    if (!NewItem || NewItem->ItemType != EItemType::Equipment || NewItem->WeaponType == EWeaponType::Ammo)
    {
        return;
    }
    
    // 퀵슬롯 컴포넌트 확인
    if (!QuickSlotComp)
    {
        return;
    }
    
    // 이미 퀵슬롯에 할당되어 있는지 확인
    bool bAlreadyAssigned = QuickSlotComp->IsItemAlreadyAssigned(NewItem);
    
    // 아직 할당되지 않았다면 첫 번째 빈 슬롯에 할당
    if (!bAlreadyAssigned)
    {
        // 첫 번째 빈 슬롯에 할당 시도
        bool bAssigned = QuickSlotComp->AssignToFirstEmptySlot(NewItem);
        
        // 모든 슬롯이 차있어서 할당 실패한 경우 첫 번째 슬롯에 강제 할당
        if (!bAssigned)
        {
            QuickSlotComp->AssignToSlot(0, NewItem);
        }
    }
    
    // 현재 슬롯 인덱스를 새 아이템이 있는 슬롯으로 설정
    int32 ItemSlotIndex = QuickSlotComp->FindSlotIndexForItem(NewItem);
    if (ItemSlotIndex != -1)
    {
        // 현재 슬롯 인덱스 업데이트
        CurrentQuickSlotIndex = ItemSlotIndex;
        QuickSlotComp->SetCurrentSlotIndex(CurrentQuickSlotIndex);
        
        // 1.4초 후 애니메이션 리셋
        if (!IsReload) // 재장전 중이 아니고
        {
            if (!EquipedWeaponComp->IsAttack) // 공격 중이 아니고
            {
                if (!IsPickUp) // 아이템 획득 중이 아니고
                {
                    if (!IsChangeAnim) // 아이템 교체 중이 아니여야지만
                    {
                        // 무기 해제 처리를 실행
                        IsChangeAnim = true;
            
                        FTimerHandle ResetAnimTimerHandle;
                        GetWorldTimerManager().SetTimer(
                            ResetAnimTimerHandle,
                            FTimerDelegate::CreateLambda([this]() { 
                                IsChangeAnim = false;
                            }),
                            1.4f,
                            false
                        );
                        return;
                    }
                }
            }
        }
        
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
// ==================================================퀵슬롯 함수 끝!==================================================================

void ANS_PlayerCharacterBase::Server_UseInventoryItem_Implementation(FName ItemRowName)
{
    for (UNS_InventoryBaseItem* Item : InventoryComp->GetInventoryContents())
    {
        if (Item && Item->ItemDataRowName == ItemRowName)
        {
            Item->OnUseItem(this);
            return;
        }
    }
}

void ANS_PlayerCharacterBase::Client_NotifyInventoryUpdated_Implementation()
{
    if (InventoryComp)
    {
        FTimerHandle DelayHandle;
        GetWorldTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([this]()
            {
                InventoryComp->OnInventoryUpdated.Broadcast();

                if (QuickSlotComp)
                {
                    QuickSlotComp->BroadcastSlotUpdate();
                }
            }), 0.05f, false);
    }
}

// 클라이언트면 서버로 클라이언트 자신에 Yaw값과 Pitch값을 서버로 전송
void ANS_PlayerCharacterBase::UpdateAim_Server_Implementation(float Yaw, float Pitch)
{
    // 모든 클라이언트에 전파
    UpdateAim_Multicast(Yaw, Pitch);
}

void ANS_PlayerCharacterBase::UpdateAim_Multicast_Implementation(float Yaw, float Pitch)
{
    // 애니메이션 업데이트를 위한 변수 설정
    CamYaw = Yaw;
    CamPitch = Pitch;
    
    // 손전등이 켜져 있다면 회전도 업데이트
    if (bFlashlightOnOff && FlashlightComp)
    {
        // 컨트롤러 회전을 직접 사용 (카메라와 동일한 방향)
        if (Controller)
        {
            FRotator ControlRotation = Controller->GetControlRotation();
            FlashlightComp->SetWorldRotation(ControlRotation);
        }
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
    FlashlightComp->SetVisibility(bFlashlightOnOff);
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
    RequestUpdateAim();
}

// 애니메이션 노티파이에서 호출할 함수 수정
void ANS_PlayerCharacterBase::OnTurnInPlaceFinished()
{
    // 서버에 회전 완료를 보고합니다.
    Server_FinishTurn();

    // 클라이언트 측에서는 즉시 리셋 상태로 전환합니다.
    bIsTurningInPlace = false;
    if (!bIsResettingYaw)
    {
        LastTurnYaw = CamYaw;
        bIsResettingYaw = true;
    }
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
}


void ANS_PlayerCharacterBase::Server_StartTurn_Implementation(bool bInTurnLeft, bool bInTurnRight)
{
    TurnLeft = bInTurnLeft;
    TurnRight = bInTurnRight;

    // 모든 클라이언트에 회전 상태를 전파합니다.
    Multicast_UpdateTurnRotation(true);
}

void ANS_PlayerCharacterBase::Server_FinishTurn_Implementation()
{
    TurnLeft = false;
    TurnRight = false;

    // 모든 클라이언트에 회전 완료 상태를 전파합니다.
    Multicast_UpdateTurnRotation(false);
}

void ANS_PlayerCharacterBase::Multicast_UpdateTurnRotation_Implementation(bool bIsTurning)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bUseControllerDesiredRotation = bIsTurning;
    }

    // 로컬 플레이어가 아닌 클라이언트의 경우, 회전 시작/종료 시 시각적 상태를 동기화합니다.
    if (!IsLocallyControlled())
    {
        bIsTurningInPlace = bIsTurning;
        if (!bIsTurning)
        {
            bIsResettingYaw = true; // 회전이 끝나면 Yaw 리셋을 시작합니다.
        }
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
    RequestUpdateAim();
}

void ANS_PlayerCharacterBase::RequestUpdateAim()
{
    // 로컬 컨트롤러가 아니면 실행하지 않음
    if (!IsLocallyControlled()) return;

    // 타이머가 설정되어 있지 않으면 새로 설정
    if (!GetWorldTimerManager().IsTimerActive(AimUpdateTimerHandle))
    {
        GetWorldTimerManager().SetTimer(AimUpdateTimerHandle, this, &ANS_PlayerCharacterBase::RequestUpdateAim, 0.1f, true);
    }

    // 마지막으로 보낸 값과 현재 값이 다를 경우에만 서버로 전송
    if (FMath::Abs(CamYaw - LastSentCamYaw) > 0.1f || FMath::Abs(CamPitch - LastSentCamPitch) > 0.1f)
    {
        LastSentCamYaw = CamYaw;
        LastSentCamPitch = CamPitch;
        UpdateAim_Server(CamYaw, CamPitch);
    }
}

void ANS_PlayerCharacterBase::ActivateHallucinationEffect()
{
    if (CameraComp && HallucinationMID)
    {
        CameraComp->AddOrUpdateBlendable(HallucinationMID, 1.f); // 활성화

        // 일정 시간 뒤 자동 비활성화
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            CameraComp->AddOrUpdateBlendable(HallucinationMID, 0.f); // 비활성화
        }, 2.0f, false);
    }
}

void ANS_PlayerCharacterBase::PlaySoundOnCharacter_Multicast_Implementation(USoundBase* SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
    }
}

void ANS_PlayerCharacterBase::Multicast_PlayPickupSound_Implementation(USoundBase* SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
    }
}

void ANS_PlayerCharacterBase::Server_SetSprinting_Implementation(bool IsSprinting)
{
    if (StatusComp)
    {
        if (IsSprinting)
        {
            StatusComp->StartSprinting();
        }
        else
        {
            StatusComp->StopSprinting();
        }
    }
}

void ANS_PlayerCharacterBase::CreateDeathBox()
{
    if (!HasAuthority() || !DeathBoxClass || !InventoryComp) return;

    // 인벤토리에서 모든 아이템 드롭
    TArray<UNS_InventoryBaseItem*> DroppedItems = InventoryComp->DropAllItems();
    
    // 아이템이 없으면 데스 박스 생성하지 않음
    if (DroppedItems.Num() == 0) return;

    // 데스 박스 생성 위치 (플레이어 발 밑)
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.bNoFail = true;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // 데스 박스 생성
    ANS_DeathBox* DeathBox = GetWorld()->SpawnActor<ANS_DeathBox>(DeathBoxClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (DeathBox)
    {
        // 아이템들을 데스 박스에 저장
        DeathBox->StoreItems(DroppedItems);
        UE_LOG(LogTemp, Warning, TEXT("[DeathBox] Created with %d items"), DroppedItems.Num());
    }
}
