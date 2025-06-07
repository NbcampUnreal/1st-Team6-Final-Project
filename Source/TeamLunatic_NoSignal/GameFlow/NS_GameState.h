#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NS_GameState.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANS_GameState();

	// 현재 밤 상태, 서버에서 계산
	UPROPERTY(ReplicatedUsing = OnRep_IsNight, BlueprintReadWrite, Category = "Time")
	bool bIsNight;

	// 현재 게임 시간 (0.0 ~ 24.0 기준)
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Time")
	float TimeOfDay;


	// Tick으로 시간 갱신
	virtual void Tick(float DeltaSeconds) override;

	// 에디터 뷰포트에서도 Tick 작동
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	UFUNCTION(BlueprintPure)
	bool IsNight() const { return bIsNight; }

protected:
	// 클라이언트에서 bIsNight 갱신 시 호출
	UFUNCTION()
	void OnRep_IsNight();

	// 복제할 변수 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 실제 하루는 24분(1440초)
	static constexpr float SecondsPerGameDay = 120.0f;

	// 1초에 흐르는 게임 내 시간량
	float TimeScale;

	// 낮/밤 판별 및 갱신
	void UpdateDayNightState();
};
