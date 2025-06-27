// Fill out your copyright notice in the Description page of Project Settings.
#include "NS_ZombieBase.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "Enum/EZombieState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enum/EZombieType.h"
#include "Kismet/GameplayStatics.h"
#include "Enum/EZombieAttackType.h"
#include "Zombie/AIController/NS_AIController.h"
#include "BrainComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NavigationInvokerComponent.h"
#if WITH_EDITOR 
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#endif 
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


ANS_ZombieBase::ANS_ZombieBase() : MaxHealth(100.f), CurrentHealth(MaxHealth), CurrentState(EZombieState::IDLE),
                                   BaseDamage(20.f),PatrolSpeed(20.f), ChaseSpeed(100.f),AccelerationSpeed(200.f),
                                   ZombieType(EZombieType::BASIC), bIsDead(false), bIsGotHit(false), SafeBones({"clavicle_r","clavicle_l","upperarm_r","upperarm_r","lowerarm_r","lowerarm_r","neck_01","head","spine_02","spine_03"})
{
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = false;

	PhysicsComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(FName("PhysicsComponent"));
	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));

	NavigationInvoker->SetGenerationRadii(1500.f, 2000.f);
	NavigationInvoker->SetAutoActivate(false);
	
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 10.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 144.0f, 0.0f);

	GetCharacterMovement()->MaxAcceleration = 350.f;         // 최대 가속도
	GetCharacterMovement()->BrakingDecelerationWalking = 200.f; // 감속도
	GetCharacterMovement()->GroundFriction = 3.0f;           // 지면 마찰력
	GetCharacterMovement()->Mass = 100.0f;                   // 질량 (높을수록 관성이 커짐)

	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->BrakingFriction = 1.0f;
	
	GetMesh()->SetRelativeLocation(FVector(0.f,0.f,-90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f,-90.f,0.f));
	
	R_SphereComp = CreateDefaultSubobject<USphereComponent>("RightAttack");
	R_SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	R_SphereComp->SetupAttachment(GetMesh(), FName("attack_r"));
	R_SphereComp->OnComponentBeginOverlap.AddDynamic(this,&ANS_ZombieBase::OnOverlapSphere);
	R_SphereComp->SetGenerateOverlapEvents(false);
	
	L_SphereComp = CreateDefaultSubobject<USphereComponent>("LeftAttack");
	L_SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	L_SphereComp->SetupAttachment(GetMesh(), FName("attack_l"));
	L_SphereComp->OnComponentBeginOverlap.AddDynamic(this,&ANS_ZombieBase::OnOverlapSphere);
	L_SphereComp->SetGenerateOverlapEvents(false);

	// bIsActive의 초기값은 false로 설정해서 처음부터 캐릭터가 활성화 거리 밖에있으면 안보이도록 설정
	bIsActive = false;
}

void ANS_ZombieBase::BeginPlay()
{
	Super::BeginPlay();

	SetActive_Multicast(false);
	SetState(CurrentState);
	
	GetMesh()->GetAnimInstance()->RootMotionMode = ERootMotionMode::RootMotionFromEverything;
	
	InitializePhysics();
	
	if (!Controller)
	{
		// AIController를 스폰
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AAIController* SpawnedController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (SpawnedController)
		{
			SpawnedController->Possess(this); // 이 캐릭터를 Possess
		}
	}
	// 서버라면 AIController의 틱도 비활성화합니다.
	if (HasAuthority())
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->SetActorTickEnabled(false);
			// 비헤이비어 트리가 있다면 여기서 Pause 또는 초기 상태로 리셋하는 로직 추가 가능
			AIController->GetBrainComponent()->PauseLogic("Reason");
		}
	}
}

void ANS_ZombieBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		PlayAnimMontage(MontageToPlay,1.f);
	}
}


void ANS_ZombieBase::OnOverlapSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (ANS_PlayerCharacterBase* Player = Cast<ANS_PlayerCharacterBase>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(Player, BaseDamage,GetController(), this, nullptr);
	}
}

void ANS_ZombieBase::SetActive_Multicast_Implementation(bool setActive)
{
	bIsActive = setActive;
	
	if (bIsActive) // 활성화 상태로 전환 (true)
	{
		if (NavigationInvoker)
		{
			NavigationInvoker->Activate(true);
		}
		SetActorHiddenInGame(false); // 액터를 숨기지 않고 보이게 함
		GetMesh()->SetVisibility(true, true); // 좀비 메쉬를 보이게 함 (자식 컴포넌트 포함)
		SetActorEnableCollision(true); // 충돌을 활성화함 (다른 액터와 상호작용 가능)
		
		// 액터 자체의 Tick 활성화
		SetActorTickEnabled(true);
		
		// 모든 컴포넌트의 Tick 활성화
		TArray<UActorComponent*> Components;
		GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			Component->SetComponentTickEnabled(true);

			// ABP 활성화
			if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Component))
			{
				if (UAnimInstance* AnimInst = SkeletalMeshComp->GetAnimInstance())
				{
					AnimInst->EnableUpdateAnimation(true);
				}
			}
		}

		// AI 컨트롤러 확인 및 재생성 로직
		if (HasAuthority())
		{
			AAIController* AIController = Cast<AAIController>(GetController());
			
			// 컨트롤러가 없으면 새로 생성
			if (!AIController && AIControllerClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);
				
				if (AIController)
				{
					AIController->Possess(this);
				}
			}
			
			// 컨트롤러가 있으면 활성화
			if (AIController)
			{
				AIController->SetActorTickEnabled(true);
				
				// BrainComponent가 있으면 로직 재개
				if (AIController->GetBrainComponent())
				{
					AIController->GetBrainComponent()->ResumeLogic("ReActivation");
				}
				TArray<UActorComponent*> ControllerComponents;
				AIController->GetComponents(ControllerComponents);
				for (UActorComponent* Component : ControllerComponents)
				{
					Component->SetComponentTickEnabled(true);
				}
				// 비헤이비어 트리 실행 (필요한 경우)
				ANS_AIController* NSAIController = Cast<ANS_AIController>(AIController);
				if (NSAIController && NSAIController->UseBlackboard(NSAIController->BehaviorTreeAsset->BlackboardAsset, NSAIController->BlackboardComp))
				{
					NSAIController->RunBehaviorTree(NSAIController->BehaviorTreeAsset);
				}
			}
		}
	}
	else // 비활성화 상태로 전환 (false)
	{
		if (NavigationInvoker)
		{
		NavigationInvoker->Activate(false);
	    }
		
		GetWorldTimerManager().ClearTimer(AmbientSoundTimer);
		SetActorHiddenInGame(true); // 액터를 숨김
		GetMesh()->SetVisibility(false, true); // 좀비 메쉬를 숨김 (자식 컴포넌트 포함)
		SetActorEnableCollision(false); // 충돌을 비활성화함 (다른 액터가 통과 가능)
		
		// 액터 자체의 Tick 비활성화
		SetActorTickEnabled(false);
		
		// 모든 컴포넌트의 Tick 비활성화
		TArray<UActorComponent*> Components;
		GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			Component->SetComponentTickEnabled(false);

			// ABP 비활성화
			if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Component))
			{
				if (UAnimInstance* AnimInst = SkeletalMeshComp->GetAnimInstance())
				{
					AnimInst->EnableUpdateAnimation(false);
				}
			}
		}

		// AI 컨트롤러가 있다면, 그 컨트롤러의 틱도 비활성화합니다.
		// 서버에서만 AI 컨트롤러의 틱을 제어하는 것이 일반적입니다.
		if (HasAuthority()) // 서버에서만 AI 컨트롤러를 제어
		{
			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				AIController->SetActorTickEnabled(false);
				// 추가적으로 비헤이비어 트리를 일시정지하는 로직을 여기에 추가할 수 있습니다.
				AIController->GetBrainComponent()->PauseLogic("Reason");
				
				// AI 컨트롤러의 모든 컴포넌트도 비활성화
				TArray<UActorComponent*> ControllerComponents;
				AIController->GetComponents(ControllerComponents);
				for (UActorComponent* Component : ControllerComponents)
				{
					Component->SetComponentTickEnabled(false);
				}
				
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Zombie %s is now fully INACTIVE."), *GetName());
	}
}

float ANS_ZombieBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHealth<=0 || !HasAuthority()) return 0.f;
	float ActualDamage = DamageAmount;

	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		const FPointDamageEvent* Point = static_cast<const FPointDamageEvent*>(&DamageEvent);
		
		FName Bone = Point->HitInfo.BoneName;
		
		FVector HitDirection = Point->ShotDirection;
		
		FVector Up = FVector::UpVector;
		FRotator HitRotation = HitDirection.Rotation();
		
		if (Bone == "head" || Bone == "neck")
		{
			ActualDamage*= 10.f;
		}
		CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.f, MaxHealth);
	
		if (CurrentHealth > 0.f)
		{
			ANS_AIController* AICon = Cast<ANS_AIController>(GetController());
			if (AICon)
			{
				AICon->GetBlackboardComponent()->SetValueAsBool("bGetHit", true);
				AICon->GetBlackboardComponent()->SetValueAsBool("bIsAttacking", false);
				AICon->GetBlackboardComponent()->SetValueAsBool("bAttackAgain", true);
			}
			GetWorldTimerManager().SetTimer(HitTimer, this, &ANS_ZombieBase::ResetHit, .5f,false);
			Multicast_PlaySound(HitSound);
			
			ApplyPhysics(Bone, HitDirection * 20000.f);
		}
		if (CurrentHealth <= 0)
		{
			GetWorldTimerManager().ClearTimer(AmbientSoundTimer);
			SetState(EZombieState::DEAD);
		}
		Multicast_SpawnBloodEffect(Bone, Point->HitInfo.Location, HitRotation);
	}
	return ActualDamage;
}

void ANS_ZombieBase::ResetHit()
{
	ANS_AIController* AICon = Cast<ANS_AIController>(GetController());
	if (AICon)
	{
		AICon->GetBlackboardComponent()->SetValueAsBool("bGetHit", false);
	}
}

void ANS_ZombieBase::ApplyPhysics_Implementation(FName Bone, FVector Impulse)
{
	if (SafeBones.Contains(Bone))
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		// FBodyInstance* BodyInstance = MeshComp->GetBodyInstance(Bone);
		// if (BodyInstance)
		// {
		// 	BodyInstance->SetInstanceSimulatePhysics(true);
		// 	BodyInstance->PhysicsBlendWeight = 1.f;
		// 	BodyInstance->AddImpulse(Impulse,true);
		// }
		GetMesh()->SetAllBodiesBelowSimulatePhysics(Bone,true,true);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(Bone, 1.f, false);
	
		GetMesh()->AddImpulse(Impulse, Bone, false);

		// if (HitTimers.Contains(Bone))
		// {
		// 	GetWorldTimerManager().ClearTimer(HitTimers[Bone]);
		// }

		// FTimerHandle& NewTimer = HitTimers.FindOrAdd(Bone);
		// GetWorldTimerManager().SetTimer(NewTimer, [this, Bone]()
		// {
		// 	ResetPhysics(Bone);
		// }, 1.f, false);
	}
}

void ANS_ZombieBase::ResetPhysics(FName Bone)
{
	if (CurrentState == EZombieState::DEAD) return;
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(Bone, 0.f, false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(Bone, false, true);
}

void ANS_ZombieBase::OnIdleState()
{
	ScheduleSound(IdleSound);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void ANS_ZombieBase::OnPatrolState()
{
	ScheduleSound(IdleSound);
	GetCharacterMovement()->MaxWalkSpeed = 70.f;
}

void ANS_ZombieBase::OnDetectState()
{

}

void ANS_ZombieBase::OnChaseState()
{
	ScheduleSound(ChaseSound);
	GetCharacterMovement()->MaxWalkSpeed =400.f;
}

void ANS_ZombieBase::OnAttackState()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetWorldTimerManager().ClearTimer(AmbientSoundTimer);
}

void ANS_ZombieBase::OnDeadState()
{
	if (bIsDead) return;
	if (HasAuthority())
	{
		Server_PlaySound(DeathSound);
		bIsDead = true;
		Die_Multicast();
	}
	
}

void ANS_ZombieBase::OnFrozenState()
{
}

void ANS_ZombieBase::Multicast_SpawnBloodEffect_Implementation(FName Bone,FVector Location, FRotator Rotation)
{
	if (BloodDecal)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AActor* Spawned = GetWorld()->SpawnActor<AActor>(BloodDecal, Location, Rotation, SpawnParams))
		{
			Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, Bone);
		}
		
	}	
}

void ANS_ZombieBase::Server_PlaySound_Implementation(USoundCue* Sound)
{
	Multicast_PlaySound(Sound);
}

void ANS_ZombieBase::Multicast_PlaySound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetMesh()->GetSocketLocation("headsocket"));
	}
}

void ANS_ZombieBase::ScheduleSound(USoundCue* SoundCue)
{
    // 유효성 검사 추가
    if (!SoundCue || !IsValid(this) || !GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("ScheduleSound: Invalid parameters or zombie is being destroyed"));
        return;
    }

    float RandomTime = FMath::FRandRange(5.f,8.f);
    GetWorldTimerManager().SetTimer(AmbientSoundTimer,[this, SoundCue]()
    {
        // 타이머 콜백 내에서도 유효성 검사
        if (!IsValid(this) || !GetWorld())
        {
            UE_LOG(LogTemp, Warning, TEXT("ScheduleSound Timer: Zombie is being destroyed"));
            return;
        }

        float PlayPercent = 0.5f;
        float ActualPercent = FMath::FRandRange(0.0f, 1.0f);
        if (PlayPercent > ActualPercent)
        {
            Server_PlaySound(SoundCue);
        }
        ScheduleSound(SoundCue);
    }, RandomTime, false);
}

void ANS_ZombieBase::Die_Multicast_Implementation()
{
	R_SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	L_SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DetachFromControllerPendingDestroy();
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()-> SetAllBodiesPhysicsBlendWeight(1.f);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(30.f);
}

void ANS_ZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_ZombieBase, bIsActive); 	// 이 좀비가 활성화 되었는지 확인 변수
	DOREPLIFETIME(ANS_ZombieBase, CurrentHealth);
	DOREPLIFETIME(ANS_ZombieBase, CurrentAttackType);
	DOREPLIFETIME(ANS_ZombieBase, CurrentState);
	DOREPLIFETIME(ANS_ZombieBase, bGetHit);
}

void ANS_ZombieBase::InitializePhysics()
{
	if (PhysicsComponent)
	{
		PhysicsComponent->SetSkeletalMeshComponent(GetMesh());

		FPhysicalAnimationData Config;
		Config.bIsLocalSimulation = true;
		Config.OrientationStrength = 50.f;
		Config.PositionStrength = 100.f;
		Config.VelocityStrength = 75.f;

		PhysicsComponent->ApplyPhysicalAnimationSettingsBelow(FName("pelvis"), Config,true) ;
	}
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetAllBodiesPhysicsBlendWeight(0.f);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}


void ANS_ZombieBase::SetState(EZombieState NewState)
{
	if (CurrentState == NewState) return;
	
	if (HasAuthority())
	{
		CurrentState = NewState;
		OnStateChanged(CurrentState);
	}
	else
	{
		Server_SetState(CurrentState);
	}
}

void ANS_ZombieBase::OnStateChanged(EZombieState State)
{
	if (State ==EZombieState::DEAD)
	{
		GetWorldTimerManager().ClearTimer(AmbientSoundTimer);
	}
	
	switch (State)
	{
	case EZombieState::IDLE:
		OnIdleState();
		break;
	case EZombieState::DEAD:
		OnDeadState();
		break;
	case EZombieState::PUSHED:
	case EZombieState::DETECTING:
		OnDetectState();
		break;
	case EZombieState::PATROLL:
		OnPatrolState();
		break;
	case EZombieState::CHACING:
		OnChaseState();
		break;
	case EZombieState::ATTACK:
		OnAttackState();
		break;
	case EZombieState::FROZEN:
		OnFrozenState();
		break;
	case EZombieState::HEARTING:
		break;
	default:
		break;
	}
}

void ANS_ZombieBase::SetAttackType(EZombieAttackType NewAttackType)
{
	if (HasAuthority())
	{
		CurrentAttackType = NewAttackType;
	} 
	else
	{
		Server_SetAttackType(NewAttackType);
	}
}


// void ANS_ZombieBase::MontagePlay(ANS_AIController* NSController, UAnimMontage* MontageToPlay)
// {
// 	if (!NSController || !MontageToPlay) return;
// 	NSController->PauseBT();
// 	
// 	float Duration = MontageToPlay->GetPlayLength();
// 	GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
// 	
// 	FTimerDelegate MontageDelegate;
// 	MontageDelegate.BindUFunction(this, FName("OnEndMontagePlay"), NSController);
// 	GetWorldTimerManager().SetTimer(MontageTimerHandle,MontageDelegate, Duration,false);
// }
//
// void ANS_ZombieBase::OnEndMontagePlay(ANS_AIController* NSController)
// {
// 	if (NSController)
// 	{
// 		NSController->ResumeBT();
// 	}
// }

void ANS_ZombieBase::Server_SetState_Implementation(EZombieState NewState)
{
	if (CurrentState == NewState) return;

	CurrentState = NewState;
	OnStateChanged(CurrentState);
}

void ANS_ZombieBase::Server_SetAttackType_Implementation(EZombieAttackType NewAttackType)
{
	if (CurrentAttackType == NewAttackType) return;
	
	CurrentAttackType = NewAttackType;
	SetAttackType(CurrentAttackType);
}

