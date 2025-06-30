// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "NS_LoadingScreen.generated.h"

/**
 * 새로운 로딩 스크린 - 단순하고 효과적인 구조
 */
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UNS_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// UI 위젯들
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_LoadingPercent;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_LoadingStatus;


	// 로딩 시작
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void StartLoading();

	// 로딩 완료 확인
	UFUNCTION(BlueprintCallable, Category = "Loading")
	bool IsLoadingComplete() const;

	// 현재 진행률 가져오기
	UFUNCTION(BlueprintCallable, Category = "Loading")
	float GetCurrentProgress() const { return CurrentProgress; }

	// 초기화
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void InitializeLoadingScreen();

private:
	// 로딩 상태
	bool bIsLoading = false;
	float CurrentProgress = 0.0f;

	// 게임 상태 체크
	bool bIsLevelLoaded = false;
	bool bIsRenderingReady = false;
	bool bIsFrameRateStable = false;

	// 프레임률 관련
	float MinRequiredFrameRate = 45.0f;
	float StableFrameCheckTime = 1.0f;
	float FrameRateCheckDuration = 0.0f;
	TArray<float> RecentFrameRates;

	// 로딩 시간 추적
	float LoadingTime = 0.0f;

	// 로딩 단계별 진행률
	void UpdateLoadingProgress();
	void UpdateUI();

	// 상태 체크 함수들
	bool CheckLevelLoaded();
	bool CheckRenderingReady();
	bool CheckFrameRateStable();

	// 로딩 완료 처리
	void OnLoadingFinished();
};
