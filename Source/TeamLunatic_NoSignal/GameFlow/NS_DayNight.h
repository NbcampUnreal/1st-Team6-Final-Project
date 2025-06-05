#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DirectionalLightComponent.h"
#include "NS_DayNight.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_DayNight : public AActor
{
	GENERATED_BODY()

public:
	ANS_DayNight();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** 현재 게임 시간 (0~24 기준) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float GameTime = 7.0f;

	/** 태양 라이트 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DayNight")
	UDirectionalLightComponent* Sun;

	/** 달 라이트 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DayNight")
	UDirectionalLightComponent* Moon;

	/** 현재 게임 시각 반환 (0~24) */
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	float GetTime() const { return GameTime; }

private:
	const float DaySpeed = 10.0f / 300.0f;   // 낮: 07~17시 → 300초
	const float NightSpeed = 14.0f / 420.0f; // 밤: 17~07시 → 420초

	bool IsDayTime() const;
	float GetRotationFromGameTime(float InGameTime) const;
};
