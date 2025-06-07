#include "NS_GameState.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"

ANS_GameState::ANS_GameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bIsNight = false;
	TimeOfDay = 17.0f; // 시작 시간: 09:00
	TimeScale = 24.0f / SecondsPerGameDay; // 1초당 게임 내 시간: 약 0.0166
}

void ANS_GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		// 안전검사: TimeScale 유효성 확인
		if (!FMath::IsFinite(TimeScale) || TimeScale <= 0.f)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid TimeScale: %f"), TimeScale);
			return;
		}

		TimeOfDay += DeltaSeconds * TimeScale;

		if (TimeOfDay >= 24.0f)
		{
			TimeOfDay -= 24.0f;
		}

		UpdateDayNightState();

		if (!FMath::IsNaN(TimeOfDay))
		{
			int32 Hour = static_cast<int32>(TimeOfDay);
			int32 Minute = static_cast<int32>((TimeOfDay - Hour) * 60.0f);
			UE_LOG(LogTemp, Log, TEXT("현재 시간은 %02d:%02d 입니다."), Hour, Minute);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TimeOfDay is NaN!"));
		}
	}
}

void ANS_GameState::UpdateDayNightState()
{
	const bool bShouldBeNight = !(TimeOfDay >= 7.5f && TimeOfDay < 18.5f);

	if (bShouldBeNight != bIsNight)
	{
		bIsNight = bShouldBeNight;
		OnRep_IsNight();
	}
}

void ANS_GameState::OnRep_IsNight()
{
	UE_LOG(LogTemp, Log, TEXT("[GameState] It is now %s."), bIsNight ? TEXT("Night") : TEXT("Day"));
}

void ANS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANS_GameState, bIsNight);
	DOREPLIFETIME(ANS_GameState, TimeOfDay);
}
