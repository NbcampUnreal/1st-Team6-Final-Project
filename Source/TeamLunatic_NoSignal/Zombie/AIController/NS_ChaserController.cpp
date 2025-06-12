#include "NS_ChaserController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFlow/NS_SinglePlayMode.h"
#include "Character/NS_PlayerController.h"

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

    if (!BlackboardComp) return;

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

    if (bIsChasing && Target)
    {
        MoveToActor(Target, 50.f, true);
    }
    else if (BlackboardComp->IsVectorValueSet(TEXT("TargetLocation")))
    {
        FVector Location = BlackboardComp->GetValueAsVector(TEXT("TargetLocation"));
        MoveToLocation(Location, 50.f, true);
    }
}

void ANS_ChaserController::RequestPlayerLocation()
{
    if (!BlackboardComp) return;

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));

    if (bIsChasing || bIsCooldown) return;

    ANS_SinglePlayMode* GameMode = Cast<ANS_SinglePlayMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        FVector Location = GameMode->GetPlayerLocation();
        BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), Location);
        UE_LOG(LogTemp, Log, TEXT("TargetLocation 갱신: %s"), *Location.ToString());
    }
}

void ANS_ChaserController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !BlackboardComp || !Actor->IsA(APawn::StaticClass())) return;
    if (!Stimulus.WasSuccessfullySensed()) return;

    const bool bIsChasing = BlackboardComp->GetValueAsBool(TEXT("IsChasingEvent"));
    const bool bIsCooldown = BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"));
    if (bIsChasing || bIsCooldown) return;

    APawn* PlayerPawn = Cast<APawn>(Actor);
    if (!PlayerPawn) return;

    SetChaseTarget(PlayerPawn, 30.0f);
    UE_LOG(LogTemp, Log, TEXT("감지로 인한 30초 추적 시작: %s"), *PlayerPawn->GetName());
}

void ANS_ChaserController::SetChaseTarget(AActor* Target, float Duration)
{
    if (!BlackboardComp || !Target) return;

    if (BlackboardComp->GetValueAsBool(TEXT("IsCooldownWait"))) return;

    BlackboardComp->SetValueAsBool(TEXT("IsChasingEvent"), true);
    BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Target);
    BlackboardComp->ClearValue(TEXT("TargetLocation")); // 좌표 기반 추적 중단

    // 클라이언트 연출
    if (APawn* PawnTarget = Cast<APawn>(Target))
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

    UE_LOG(LogTemp, Warning, TEXT("추적 종료, 쿨다운 시작"));
}
