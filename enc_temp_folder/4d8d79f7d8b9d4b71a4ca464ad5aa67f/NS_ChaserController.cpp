#include "NS_ChaserController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Character/NS_PlayerController.h"
#include "GameFlow/NS_SinglePlayMode.h"

ANS_ChaserController::ANS_ChaserController()
{
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

void ANS_ChaserController::RequestPlayerLocation()
{
    if (!BlackboardComp) return;

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));

    if (bIsChasing)
    {
        UE_LOG(LogTemp, Log, TEXT("추적 중이라 좌표 갱신 생략"));
        return;
    }

    if (bIsCooldown)
    {
        UE_LOG(LogTemp, Log, TEXT("쿨다운 상태라 좌표 갱신 생략"));
        return;
    }

    ANS_SinglePlayMode* GameMode = Cast<ANS_SinglePlayMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        FVector Location = GameMode->GetPlayerLocation();
        BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), Location);
        UE_LOG(LogTemp, Log, TEXT(" TargetLocation 갱신: %s"), *Location.ToString());
    }
}

void ANS_ChaserController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !BlackboardComp || !Actor->IsA(APawn::StaticClass())) return;
    if (!Stimulus.WasSuccessfullySensed()) return;

    AActor* PendingTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("PendingTarget")));
    const float PendingDuration = BlackboardComp->GetValueAsFloat(TEXT("PendingChaseDuration"));

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));

    if (PendingTarget && Actor == PendingTarget)
    {
        BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), true);
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PendingTarget);
        BlackboardComp->ClearValue(TEXT("TargetLocation"));
        BlackboardComp->ClearValue(TEXT("PendingTarget"));
        BlackboardComp->ClearValue(TEXT("PendingChaseDuration"));

        MoveToActor(PendingTarget, 100.f, true);

        // 추적당하는 플레이어에게 클라이언트 알림 전송
        if (APawn* PawnTarget = Cast<APawn>(PendingTarget))
        {
            if (APlayerController* PC = Cast<APlayerController>(PawnTarget->GetController()))
            {
                if (ANS_PlayerController* APC = Cast<ANS_PlayerController>(PC))
                {
                    APC->PlayTracked_Implementation();
                }
            }
        }

        GetWorld()->GetTimerManager().ClearTimer(ChaseResetTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(
            ChaseResetTimerHandle,
            this,
            &ANS_ChaserController::ResetChase,
            PendingDuration,
            false
        );

        UE_LOG(LogTemp, Warning, TEXT("시야 감지 → 추적 시작: %s (%.1f초)"), *PendingTarget->GetName(), PendingDuration);
        return;
    }


    // 평상시 감지 반응 (이미 IsChasingEvent false 상태에서 감지된 경우만)
    if (!bIsChasing && !bIsCooldown)
    {
        SetChaseTarget(Actor, 10.0f); // 기본 감지용 추적
        UE_LOG(LogTemp, Log, TEXT(" 감지로 인한 기본 추적 시작: %s"), *Actor->GetName());
    }
}


void ANS_ChaserController::SetChaseTarget(AActor* Target, float Duration)
{
    if (!BlackboardComp || !Target) return;

    if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait")))
    {
        UE_LOG(LogTemp, Log, TEXT("쿨다운 중 - 추적 무시"));
        return;
    }

    BlackboardComp->SetValueAsObject(TEXT("PendingTarget"), Target); // 일단 보류 타겟 저장
    BlackboardComp->SetValueAsFloat(TEXT("PendingChaseDuration"), Duration);

    UE_LOG(LogTemp, Warning, TEXT(" 추적 대기: 타겟 %s, 시야 감지될 때까지 대기"), *Target->GetName());
}


void ANS_ChaserController::ResetChase()
{
    if (!BlackboardComp) return;

    BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
    BlackboardComp->ClearValue(TEXT("TargetActor"));

    BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), true);
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,
        [this]()
    {
        BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), false);
        UE_LOG(LogTemp, Log, TEXT(" 쿨다운 종료"));
    },
        30.0f,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT(" 추적 종료, 쿨다운 시작"));
}


