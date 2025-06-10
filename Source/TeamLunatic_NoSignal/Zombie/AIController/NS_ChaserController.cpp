// ChaserZombieController.cp
#include "ChaserZombieController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AChaserZombieController::AChaserZombieController()
{
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 600.f; // 시야 반경
    SightConfig->LoseSightRadius = 650.f;
    SightConfig->PeripheralVisionAngleDegrees = 70.f; // 시야각

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComp->ConfigureSense(*SightConfig);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 900.f; // 소리 감지 반경
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComp->ConfigureSense(*HearingConfig);

    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AChaserZombieController::OnPerceptionUpdated);
}

void AChaserZombieController::BeginPlay()
{
    Super::BeginPlay();
    // 디버깅 로그 등 추가 가능
}

void AChaserZombieController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        if (Stimulus.Type == UAISense_Sight::StaticClass()->GetFName())
        {
            UE_LOG(LogTemp, Warning, TEXT("Sight: %s 감지!"), *Actor->GetName());
        }
        if (Stimulus.Type == UAISense_Hearing::StaticClass()->GetFName())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hearing: %s의 소리 감지!"), *Actor->GetName());
        }
    }
}
