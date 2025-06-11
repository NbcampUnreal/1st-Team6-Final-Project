#include "NS_ChaserController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_SinglePlayMode.h"

ANS_ChaserController::ANS_ChaserController()
{
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    SightConfig->SightRadius = 600.f;
    SightConfig->LoseSightRadius = 650.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    PerceptionComp->ConfigureSense(*SightConfig);

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

    if (BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent")))
    {
        UE_LOG(LogTemp, Log, TEXT("추적 중이라 좌표 갱신 생략"));
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

    const bool bSensed = Stimulus.WasSuccessfullySensed();
    APawn* PlayerPawn = Cast<APawn>(Actor);
    if (!PlayerPawn || !bSensed) return;

    // 감지되었으면 → 30초 추적 시작
    SetChaseTarget(PlayerPawn, 30.0f);

    UE_LOG(LogTemp, Log, TEXT(" 감지로 인한 30초 추적 시작: %s"), *PlayerPawn->GetName());
}

void ANS_ChaserController::SetChaseTarget(AActor* Target, float Duration)
{
    if (!BlackboardComp || !Target) return;

    // 쿨다운 중이면 무시
    if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait")))
    {
        UE_LOG(LogTemp, Log, TEXT("쿨다운 중 - 추적 무시"));
        return;
    }

    BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), true);
    BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Target);
    BlackboardComp->ClearValue(TEXT("TargetLocation"));

    MoveToActor(Target, 100.f, true);

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

    BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), false);
    BlackboardComp->ClearValue(TEXT("TargetActor"));

    // 쿨다운 시작
    BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), true);
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,
        [this]()
    {
        BlackboardComp->SetValueAsBool(TEXT("IsCooldownWait"), false);
        UE_LOG(LogTemp, Warning, TEXT(" 추적 쿨다운 종료"));
    },
        10.0f, // 쿨다운 지속 시간
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("추적 종료 → 쿨다운 시작"));
}
