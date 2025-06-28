// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NS_LoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void LevelLoadComplete()
	{
		bIsLevelLoadComplete = true;
	}
	void FakeUpdateProgress();
	//UFUNCTION()
	void UpdateProgress();

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_Loading;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_LoadingPercent;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_LoadingStatus;

	float FakeProgress = 0.f;
	float FakeProgressMax = 15.f; // 15초 동안 로딩이 진행된다고 가정
	bool bIsLevelLoadComplete = false;
	FTimerHandle LoadingTickHandle;

	// 실제 게임 상태 체크 관련 변수들
	bool bIsRenderingReady = false;
	bool bIsFrameRateStable = false;
	float FrameRateCheckDuration = 0.f;
	float MinRequiredFrameRate = 45.0f; // 최소 요구 프레임률 (더 부드러운 게임플레이를 위해)
	float StableFrameCheckTime = 1.0f; // 안정적인 프레임을 확인할 시간 (초)
	TArray<float> RecentFrameRates; // 최근 프레임률 기록

	// 실제 게임 준비 상태 체크 함수들
	bool CheckRenderingReadiness();
	bool CheckFrameRateStability();
	void UpdateRealLoadingProgress();

};
