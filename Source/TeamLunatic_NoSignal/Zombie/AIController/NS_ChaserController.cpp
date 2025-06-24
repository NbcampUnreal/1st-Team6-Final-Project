#include "NS_ChaserController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_GameModeBase.h"
#include "GameFlow/NS_GameState.h"
#include "Zombie/NS_ZombieBase.h" 
#include "Zombie/Enum/EZombieState.h"
#include "Character/NS_PlayerController.h"
#include "Character/NS_PlayerCharacterBase.h"

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
}

void ANS_ChaserController::BeginPlay()
{
    Super::BeginPlay();

    if (UseBlackboard(BlackboardAsset, BlackboardComp))
    {
        RunBehaviorTree(BehaviorTreeAsset);
        BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
        BlackboardComp->ClearValue(TEXT("TargetActor"));
    }
    PerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(this, &ANS_ChaserController::OnPerceptionUpdated);
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ANS_ChaserController::OnPerceptionUpdated);
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
        return;
    }

    if (bIsChasing && Target)
    {
        MoveToActor(Target, 50.f, true);

        const float Distance = FVector::Dist(Target->GetActorLocation(), GetPawn()->GetActorLocation());
        if (Distance < 300.f)
        {
            if (!bIsDealingDamage)
                StartDamageLoop(Target);
        }
        else
        {
            if (bIsDealingDamage)
                StopDamageLoop();
        }
    }
    else if (BlackboardComp->IsVectorValueSet(TEXT("TargetLocation")))
    {
        FVector Location = BlackboardComp->GetValueAsVector(TEXT("TargetLocation"));
        MoveToLocation(Location, 50.f, true);
    }
}


void ANS_ChaserController::RequestPlayerLocation()
{
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("RequestPlayerLocation: BlackboardComp 없음"));
        return;
    }

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));

    if (bIsChasing || bIsCooldown)
    {
        UE_LOG(LogTemp, Log, TEXT("추적 중 또는 쿨다운 중이라 위치 갱신 생략"));
        return;
    }

    ANS_GameModeBase* GameMode = Cast<ANS_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode 캐스팅 실패"));
        return;
    }

    FVector Location = GameMode->GetPlayerLocation();

    if (Location.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("플레이어 위치가 ZeroVector로 반환됨"));

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
                UE_LOG(LogTemp, Warning, TEXT("모든 플레이어가 사망하여 위치 요청 중지"));
                return;
            }
        }

        // 0.5초 뒤 재요청
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, this, &ANS_ChaserController::RequestPlayerLocation, 0.5f, false);
        return;
    }

    // 유효한 위치면 블랙보드에 저장
    UE_LOG(LogTemp, Log, TEXT("플레이어 위치 갱신됨: %s"), *Location.ToString());
    BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), Location);
}

void ANS_ChaserController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !BlackboardComp || !Actor->IsA(APawn::StaticClass())) return;
    if (!Stimulus.WasSuccessfullySensed()) return;

    // 감지된 Actor가 NS_PlayerCharacterBase가 아니면 무시
    if (!Actor->IsA(ANS_PlayerCharacterBase::StaticClass()))
    {
        return;
    }

    // 쿨다운 중이면 무시
    if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait")))
    {
        return;
    }

    // 이미 추격 중이면 무시
    if (BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent")))
    {
        return;
    }

    if (!BlackboardComp->GetValueAsBool(TEXT("IsActivated"))) // 아직 활성화되지 않았다면
    {
        BlackboardComp->SetValueAsBool(TEXT("IsActivated"), true); // 활성화 상태로 변경
        UE_LOG(LogTemp, Log, TEXT("Chaser 활성화됨: 플레이어 처음 감지됨!"));
    }

    // 추격 시작
    APawn* PlayerPawn = Cast<APawn>(Actor);
    if (!PlayerPawn) return;

    SetChaseTarget(PlayerPawn, 30.0f);
    UE_LOG(LogTemp, Log, TEXT("감지로 인한 30초 추적 시작: %s"), *PlayerPawn->GetName());
}


void ANS_ChaserController::SetChaseTarget(AActor* Target, float Duration)
{
    // 서버에서만 로직 실행 (ChaserController는 서버에서만 동작해야 함)
    if (!HasAuthority() || !BlackboardComp || !Target) return;

    if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"))) return;

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
                UE_LOG(LogTemp, Error, TEXT("소리재생해라"));
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

    UE_LOG(LogTemp, Warning, TEXT("강제 추적 시작: %s (%.1f초)"), *Target->GetName(), Duration);
}
void ANS_ChaserController::ResetChase()
{
    if (!BlackboardComp) return;

    if (ANS_ZombieBase* ChaserPawn = Cast<ANS_ZombieBase>(GetPawn()))
    {
        ChaserPawn->SetState(EZombieState::IDLE);
    }

    BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
    BlackboardComp->ClearValue(TEXT("TargetActor"));

    BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), true);
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,
        [this]()
    {
        BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), false);
        UE_LOG(LogTemp, Log, TEXT("쿨다운 종료"));

    },
        30.0f,
        false
    );

    StopDamageLoop();

    UE_LOG(LogTemp, Warning, TEXT("추적 종료, 쿨다운 시작"));
}


void ANS_ChaserController::StartDamageLoop(AActor* Target)
{
    if (!Target || bIsDealingDamage) return;

    DamageTarget = Target;
    bIsDealingDamage = true;

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
    GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
    bIsDealingDamage = false;
    DamageTarget = nullptr;
}

void ANS_ChaserController::ApplyDamageToTarget()
{
    if (!DamageTarget) return;

    if (ANS_PlayerCharacterBase* PlayerChar = Cast<ANS_PlayerCharacterBase>(DamageTarget))
    {
        // 데미지 적용
        UGameplayStatics::ApplyDamage(PlayerChar, 10.0f, this, GetPawn(), nullptr);

        // 환각 효과 발동
        if (ANS_PlayerCharacterBase* PlayerCharacter = Cast<ANS_PlayerCharacterBase>(DamageTarget))
        {
            UGameplayStatics::ApplyDamage(PlayerCharacter, 10.0f, this, GetPawn(), nullptr);

            // PlayerCharacter->ActivateHallucinationEffect(3.0f); // 3초간 환각
        }

    }
}

