#include "NS_DayNight.h"

ANS_DayNight::ANS_DayNight()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(RootComponent);

	Moon = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Moon"));
	Moon->SetupAttachment(RootComponent);
}

void ANS_DayNight::BeginPlay()
{
	Super::BeginPlay();
}

void ANS_DayNight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 낮/밤 판별
	const bool bIsDay = IsDayTime();
	const float Speed = bIsDay ? DaySpeed : NightSpeed;

	// 시간 누적
	GameTime += DeltaTime * Speed;
	if (GameTime >= 24.0f)
	{
		GameTime -= 24.0f;
	}

	// 각도 계산
	const float SunAngle = GetRotationFromGameTime(GameTime);
	const float MoonAngle = FMath::Fmod(SunAngle + 180.0f, 360.0f);

	const FRotator SunRotation = FRotator(SunAngle, 0.f, 0.f);
	const FRotator MoonRotation = FRotator(MoonAngle, 0.f, 0.f);

	// 회전 적용
	if (Sun)
	{
		Sun->SetWorldRotation(SunRotation);
		Sun->SetVisibility(bIsDay);
	}

	if (Moon)
	{
		Moon->SetWorldRotation(MoonRotation);
		Moon->SetVisibility(!bIsDay);
	}
}

bool ANS_DayNight::IsDayTime() const
{
	return (GameTime >= 7.0f && GameTime < 17.0f);
}

float ANS_DayNight::GetRotationFromGameTime(float InGameTime) const
{
	return (InGameTime / 24.0f) * 360.0f;
}
