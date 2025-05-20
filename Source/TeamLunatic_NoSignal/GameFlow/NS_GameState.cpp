#include "NS_GameState.h"
#include "EngineUtils.h"

#include "Kismet/GameplayStatics.h"


ANS_GameState::ANS_GameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANS_GameState::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		DirectionalLight = *It;
		//UE_LOG(LogTemp, Warning, TEXT("[DayNight] Directional Light Found: %s"), *DirectionalLight->GetName());
		break;
	}

	if (!DirectionalLight)
	{
		//UE_LOG(LogTemp, Error, TEXT("[DayNight] No Directional Light found in level!"));
	}
}

void ANS_GameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!DirectionalLight)
    {
       //UE_LOG(LogTemp, Error, TEXT("[DayNight] DirectionalLight is null in Tick()"));
        return;
    }

	static float AccumulatedPitch = 90.0f; // 시작 각도 고정

	AccumulatedPitch += RotationSpeedDegreesPerSec * DeltaTime;
	if (AccumulatedPitch >= 360.f)
		AccumulatedPitch -= 360.f;

	FRotator NewRotation = FRotator(AccumulatedPitch, 0.f, 0.f);
	DirectionalLight->SetActorRotation(NewRotation);

	//UE_LOG(LogTemp, Warning, TEXT("FORCED ROTATION -> Pitch = %.2f"), AccumulatedPitch);

}
