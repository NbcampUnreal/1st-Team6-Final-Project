#include "NS_ChaserController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameModeBase.h"
#include "GameFlow/NS_GameState.h"
#include "Zombie/NS_ZombieBase.h"
#include "Zombie/Enum/EZombieState.h"
#include "Character/NS_PlayerController.h"
#include "Character/NS_PlayerCharacterBase.h"
#include "NavigationSystem.h" // UNavigationSystemV1을 위해 추가

ANS_ChaserController::ANS_ChaserController()
{
	PrimaryActorTick.bCanEverTick = true;

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));



	// 시야 설정
	SightConfig->SightRadius = 600.f;
	SightConfig->LoseSightRadius = 650.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComp->ConfigureSense(*SightConfig);

	// 청각 설정
	HearingConfig->HearingRange = 900.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComp->ConfigureSense(*HearingConfig);

	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	// 워프 거리 기본값 설정
	WarpDistance = 3000.0f;
}

void ANS_ChaserController::BeginPlay()
{
	Super::BeginPlay();

	if (UseBlackboard(BlackboardAsset, BlackboardComp))
	{
		RunBehaviorTree(BehaviorTreeAsset);
		BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
		BlackboardComp->ClearValue(TEXT("TargetActor"));
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] BeginPlay: Behavior Tree and Blackboard initialized."));
	}
	PerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(this, &ANS_ChaserController::OnPerceptionUpdated);
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_ChaserController::OnPerceptionUpdated);
	UE_LOG(LogTemp, Warning, TEXT("[ChaserController] BeginPlay: Perception Updated delegate bound."));
}

void ANS_ChaserController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || !BlackboardComp) return;

	const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
	const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

	if (bIsCooldown)
	{
		// 쿨다운 중이면 강제로 멈춤
		StopMovement();
		StopDamageLoop();
		// UE_LOG(LogTemp, Verbose, TEXT("[ChaserController] Tick: In Cooldown. Stopping movement."));
		return;
	}

	if (bIsChasing && Target)
	{
		MoveToActor(Target, 50.f, true);
		// UE_LOG(LogTemp, Verbose, TEXT("[ChaserController] Tick: Chasing Target. Moving to %s"), *Target->GetName());

		const float Distance = FVector::Dist(Target->GetActorLocation(), GetPawn()->GetActorLocation());
		if (Distance < 300.f)
		{
			if (!bIsDealingDamage)
			{
				StartDamageLoop(Target);
				UE_LOG(LogTemp, Log, TEXT("[ChaserController] Tick: Target in range (<300cm). Starting damage loop."));
			}
		}
		else
		{
			if (bIsDealingDamage)
			{
				StopDamageLoop();
				UE_LOG(LogTemp, Log, TEXT("[ChaserController] Tick: Target out of range. Stopping damage loop."));
			}
		}
	}
	else if (BlackboardComp->IsVectorValueSet(TEXT("TargetLocation")))
	{
		FVector Location = BlackboardComp->GetValueAsVector(TEXT("TargetLocation"));
		MoveToLocation(Location, 50.f, true);
		// UE_LOG(LogTemp, Verbose, TEXT("[ChaserController] Tick: Moving to TargetLocation %s"), *Location.ToString());
	}
}


void ANS_ChaserController::RequestPlayerLocation()
{
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChaserController] RequestPlayerLocation: BlackboardComp is null."));
		return;
	}

	const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
	const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));

	if (bIsChasing || bIsCooldown)
	{
		UE_LOG(LogTemp, Log, TEXT("[ChaserController] RequestPlayerLocation: Skipping update because AI is either chasing or in cooldown."));
		return;
	}

	ANS_GameModeBase* GameMode = Cast<ANS_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChaserController] RequestPlayerLocation: GameMode casting failed."));
		return;
	}

	FVector Location = GameMode->GetPlayerLocation();

	if (Location.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] RequestPlayerLocation: Player location returned as ZeroVector. Retrying in 0.5s."));

		// 살아있는 플레이어가 아무도 없다면 재요청 불필요
		if (ANS_GameState* GS = GetWorld()->GetGameState<ANS_GameState>())
		{
			int32 AliveCount = 0;
			for (APlayerState* PS : GS->PlayerArray)
			{
				if (ANS_MainGamePlayerState* MPS = Cast<ANS_MainGamePlayerState>(PS))
				{
					if (MPS->bIsAlive)
					{
						AliveCount++;
						break;
					}
				}
			}

			if (AliveCount == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("[ChaserController] All players are dead, stopping location requests."));
				return;
			}
		}

		// 0.5초 뒤 재요청
		FTimerHandle RetryHandle;
		GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &ANS_ChaserController::RequestPlayerLocation, 0.5f, false);
		return;
	}

	// 유효한 위치면 블랙보드에 저장
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Player location updated to: %s"), *Location.ToString());
	BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), Location);
}

void ANS_ChaserController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지 이벤트 발생 로그
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Perception Updated: Actor = %s, Stimulus Sensed = %s"), *Actor->GetName(), (Stimulus.WasSuccessfullySensed() ? TEXT("True") : TEXT("False")));

	if (!Actor || !BlackboardComp || !Actor->IsA(APawn::StaticClass())) return;

	// 감지된 Actor가 NS_PlayerCharacterBase가 아니면 무시
	if (!Actor->IsA(ANS_PlayerCharacterBase::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[ChaserController] Perception Updated: Actor is not a player character. Ignoring."));
		return;
	}

	// 감지에 성공했을 때만 로직 실행
	if (Stimulus.WasSuccessfullySensed())
	{
		// 쿨다운 중이면 무시
		if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ChaserController] OnPerceptionUpdated: In cooldown, ignoring new sense."));
			return;
		}

		// 이미 추격 중이면 무시
		if (BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[ChaserController] OnPerceptionUpdated: Already chasing, ignoring new sense."));
			return;
		}

		if (!BlackboardComp->GetValueAsBool(TEXT("IsActivated"))) // 아직 활성화되지 않았다면
		{
			BlackboardComp->SetValueAsBool(TEXT("IsActivated"), true); // 활성화 상태로 변경
			UE_LOG(LogTemp, Log, TEXT("[ChaserController] Chaser activated: Player detected for the first time!"));
		}

		// 추격 시작
		APawn* PlayerPawn = Cast<APawn>(Actor);
		if (!PlayerPawn) return;

		SetChaseTarget(PlayerPawn, 30.0f);
		UE_LOG(LogTemp, Log, TEXT("[ChaserController] Starting 30-second chase due to perception: %s"), *PlayerPawn->GetName());
	}
	else
	{
		// 감지를 잃었을 때 (LoseSense)
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] Lost perception of Actor: %s"), *Actor->GetName());
		// 감지 손실 시에도 강제 추적 타이머는 계속 유지됩니다.
	}
}


void ANS_ChaserController::SetChaseTarget(AActor* Target, float Duration)
{
	// 서버에서만 로직 실행 (ChaserController는 서버에서만 동작해야 함)
	if (!HasAuthority() || !BlackboardComp || !Target) return;

	if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait")))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] SetChaseTarget: Cannot set chase target while in cooldown."));
		return;
	}

	if (ANS_ZombieBase* ChaserPawn = Cast<ANS_ZombieBase>(GetPawn()))
	{
		ChaserPawn->SetState(EZombieState::CHACING);
	}

	BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), true);
	BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Target);
	BlackboardComp->ClearValue(TEXT("TargetLocation")); // 좌표 기반 추적 중단

	// 클라이언트에 소리 키기
	if (APawn* PawnTarget = Cast<APawn>(Target))
	{
		if (APlayerController* PC = Cast<APlayerController>(PawnTarget->GetController()))
		{
			if (ANS_PlayerController* APC = Cast<ANS_PlayerController>(PC))
			{
				UE_LOG(LogTemp, Error, TEXT("[ChaserController] APC->PlayTracked() called."));
				APC->PlayTracked();
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ChaseResetTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		ChaseResetTimerHandle,
		this,
		&ANS_ChaserController::ResetChase,
		Duration,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[ChaserController] Forced chase started on %s for %.1f seconds."), *Target->GetName(), Duration);
}


void ANS_ChaserController::ResetChase()
{
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChaserController] ResetChase: BlackboardComp is null."));
		return;
	}

	if (ANS_ZombieBase* ChaserPawn = Cast<ANS_ZombieBase>(GetPawn()))
	{
		ChaserPawn->SetState(EZombieState::IDLE);
	}

	BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
	BlackboardComp->ClearValue(TEXT("TargetActor"));

	// 쿨다운 시작
	BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), true);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&ANS_ChaserController::WarpToPlayerLocation,
		15.0f,
		false
	);

	StopDamageLoop();

	UE_LOG(LogTemp, Warning, TEXT("[ChaserController] Chase ended. Starting 15-second cooldown and warp."));
}

void ANS_ChaserController::WarpToPlayerLocation()
{
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChaserController] WarpToPlayerLocation: BlackboardComp is null."));
		return;
	}

	BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), false);
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] WarpToPlayerLocation: Cooldown finished. Starting warp location calculation."));

	ANS_GameModeBase* GameMode = Cast<ANS_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChaserController] WarpToPlayerLocation: GameMode casting failed. Aborting warp."));
		RequestPlayerLocation(); // Fallback to normal location request
		return;
	}

	FVector PlayerLocation = GameMode->GetPlayerLocation();
	FVector ZombieLocation = GetPawn()->GetActorLocation();

	// 플레이어 위치가 유효한지 확인
	if (PlayerLocation.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] WarpToPlayerLocation: Player location is ZeroVector. Aborting warp."));
		RequestPlayerLocation(); // Fallback
		return;
	}

	FVector DirectionToPlayer = (PlayerLocation - ZombieLocation).GetSafeNormal();
	FVector WarpTargetLocation = PlayerLocation - (DirectionToPlayer * WarpDistance);

	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Calculated raw warp location: %s (Player: %s, Zombie: %s)"), *WarpTargetLocation.ToString(), *PlayerLocation.ToString(), *ZombieLocation.ToString());

	FNavLocation NavLoc;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSys && NavSys->ProjectPointToNavigation(WarpTargetLocation, NavLoc))
	{
		// NavMesh 위 유효한 위치로 워프
		GetPawn()->SetActorLocation(NavLoc.Location);
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] WARP SUCCESS! New location: %s"), *NavLoc.Location.ToString());
		RequestPlayerLocation(); // 워프 후 다시 이동 목표 설정
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChaserController] WARP FAILED! Could not find a valid location on NavMesh near %s."), *WarpTargetLocation.ToString());
		RequestPlayerLocation(); // 워프 실패 시 기존 방식으로 계속 시도
	}
}


void ANS_ChaserController::StartDamageLoop(AActor* Target)
{
	if (!Target || bIsDealingDamage) return;

	DamageTarget = Target;
	bIsDealingDamage = true;
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Starting damage loop on target: %s"), *Target->GetName());

	GetWorld()->GetTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&ANS_ChaserController::ApplyDamageToTarget,
		0.5f,
		true
	);
}

void ANS_ChaserController::StopDamageLoop()
{
	if (!bIsDealingDamage) return;
	GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
	bIsDealingDamage = false;
	DamageTarget = nullptr;
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Stopping damage loop."));
}

void ANS_ChaserController::ApplyDamageToTarget()
{
	if (!DamageTarget)
	{
		StopDamageLoop();
		return;
	}

	// 데미지 적용
	float DamageAmount = 10.0f;
	UGameplayStatics::ApplyDamage(DamageTarget, DamageAmount, this, GetPawn(), nullptr);
	UE_LOG(LogTemp, Log, TEXT("[ChaserController] Applied %f damage to %s"), DamageAmount, *DamageTarget->GetName());

	// 환각 효과 발동
	if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(DamageTarget))
	{
		PlayerCharacter->ActivateHallucinationEffect(3.0f); // 3초간 환각
		UE_LOG(LogTemp, Log, TEXT("[ChaserController] Activated hallucination effect on player."));
	}
}