// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/NS_LoadingScreen.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"

void UNS_LoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();

	// 에디터에서 실행 중인지 확인
	bool bIsInEditor = GIsEditor;
	UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 생성 - 에디터 모드: %s"), bIsInEditor ? TEXT("예") : TEXT("아니오"));

	// 배경을 완전 불투명한 검은색으로 설정하여 화면을 완전히 가림
	if (Image_Background)
	{
		Image_Background->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)); // 완전 불투명
		Image_Background->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 배경 이미지 설정 완료 - 완전 불투명"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Image_Background가 null입니다! 코드로 강제 생성"));

		// 배경이 없다면 코드로 강제 생성
		Image_Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		if (Image_Background)
		{
			Image_Background->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)); // 완전 불투명

			// 전체 화면을 덮도록 설정
			UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
			WidgetTree->RootWidget = RootCanvas;

			UCanvasPanelSlot* BackgroundSlot = RootCanvas->AddChildToCanvas(Image_Background);
			if (BackgroundSlot)
			{
				BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f)); // 전체 화면
				BackgroundSlot->SetOffsets(FMargin(0.0f)); // 여백 없음
			}

			UE_LOG(LogTemp, Warning, TEXT("코드로 전체 화면 배경 생성 완료"));
		}
	}

	// 위젯 자체도 강제로 보이게 설정
	SetVisibility(ESlateVisibility::Visible);

	InitializeLoadingScreen();
}

void UNS_LoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bIsLoading)
	{
		UpdateLoadingProgress();
		UpdateUI();
	}
}

void UNS_LoadingScreen::StartLoading()
{
	bIsLoading = true;
	CurrentProgress = 0.0f;
	bIsLevelLoaded = false;
	bIsRenderingReady = false;
	bIsFrameRateStable = false;
	FrameRateCheckDuration = 0.0f;
	RecentFrameRates.Empty();
	LoadingTime = 0.0f; // 로딩 시간 초기화 (0%에서 시작)

	UE_LOG(LogTemp, Warning, TEXT("로딩 시작 - 0%%에서 시작 - 뷰포트에 있는지 확인: %s"), IsInViewport() ? TEXT("예") : TEXT("아니오"));

	// 뷰포트에 없다면 다시 추가
	if (!IsInViewport())
	{
		AddToViewport(32767); // 최대 Z-Order로 모든 것을 가림
		UE_LOG(LogTemp, Warning, TEXT("로딩 스크린을 최상위 Z-Order로 뷰포트에 추가함"));
	}

	// 강제로 보이게 설정
	SetVisibility(ESlateVisibility::Visible);

	UE_LOG(LogTemp, Warning, TEXT("로딩 시작 완료"));
}

void UNS_LoadingScreen::InitializeLoadingScreen()
{
	CurrentProgress = 0.0f;
	
	if (Text_LoadingPercent)
	{
		Text_LoadingPercent->SetText(FText::FromString(TEXT("0%%")));
	}
	
	if (Text_LoadingStatus)
	{
		Text_LoadingStatus->SetText(FText::FromString(TEXT("로딩 중...")));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 초기화 완료"));
}

bool UNS_LoadingScreen::IsLoadingComplete() const
{
	return bIsLevelLoaded && bIsRenderingReady && bIsFrameRateStable;
}

void UNS_LoadingScreen::UpdateLoadingProgress()
{
	// 새로운 진행률 계산: 0% → 70% (시간 기반) → 100% (프레임률 안정화)
	float NewProgress = 0.0f;

	// 시간 기반으로 진행률 계산
	LoadingTime += GetWorld()->GetDeltaSeconds();

	if (LoadingTime < 5.0f)
	{
		// 0% ~ 70%: 5초에 걸쳐 시간 기반으로 천천히 증가 (렌더링 시뮬레이션)
		float TimeProgress = FMath::Clamp(LoadingTime / 5.0f, 0.0f, 1.0f);
		NewProgress = TimeProgress * 0.7f;
	}
	else
	{
		// 70% 이후: 실제 프레임률 체크 시작
		NewProgress = 0.7f;

		// 레벨과 렌더링이 준비되었고 프레임률도 안정적이면 100%
		if (CheckLevelLoaded() && CheckRenderingReady() && CheckFrameRateStable())
		{
			NewProgress = 1.0f;
		}
		else if (CheckLevelLoaded() && CheckRenderingReady())
		{
			// 프레임률 안정화 진행 중 (70% ~ 100%)
			float FrameProgress = FMath::Clamp(FrameRateCheckDuration / StableFrameCheckTime, 0.0f, 1.0f);
			NewProgress = 0.7f + (FrameProgress * 0.3f);
		}
	}

	// 진행률이 뒤로 가지 않도록 보장
	CurrentProgress = FMath::Max(CurrentProgress, NewProgress);
	CurrentProgress = FMath::Clamp(CurrentProgress, 0.0f, 1.0f);

	// 로딩 완료 체크 (70% 이후에만)
	if (LoadingTime >= 5.0f && IsLoadingComplete())
	{
		OnLoadingFinished();
	}
}

void UNS_LoadingScreen::UpdateUI()
{
	if (Text_LoadingPercent)
	{
		FString PercentText = FString::Printf(TEXT("%.0f%%"), CurrentProgress * 100.0f);
		Text_LoadingPercent->SetText(FText::FromString(PercentText));
	}
	
	if (Text_LoadingStatus)
	{
		FString StatusText;
		if (LoadingTime < 5.0f)
		{
			// 0% ~ 70%: 시간 기반 렌더링 시뮬레이션
			if (CurrentProgress < 0.2f)
			{
				StatusText = TEXT("게임 시작 중...");
			}
			else if (CurrentProgress < 0.5f)
			{
				StatusText = TEXT("에셋 로딩 중...");
			}
			else
			{
				StatusText = TEXT("렌더링 준비 중...");
			}
		}
		else if (!bIsFrameRateStable)
		{
			// 70% ~ 100%: 실제 프레임률 안정화
			float CurrentFPS = 0.0f;
			if (RecentFrameRates.Num() > 0)
			{
				CurrentFPS = RecentFrameRates.Last();
			}
			StatusText = FString::Printf(TEXT("프레임률 안정화 중... (현재: %.0f FPS)"), CurrentFPS);
		}
		else
		{
			StatusText = TEXT("로딩 완료!");
		}

		Text_LoadingStatus->SetText(FText::FromString(StatusText));
	}
}

bool UNS_LoadingScreen::CheckLevelLoaded()
{
	if (bIsLevelLoaded) return true;
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	// 월드가 완전히 로드되었는지 확인
	bool bActorsInitialized = World->AreActorsInitialized();
	bool bHasBegunPlay = World->HasBegunPlay();
	
	if (bActorsInitialized && bHasBegunPlay)
	{
		bIsLevelLoaded = true;
		UE_LOG(LogTemp, Warning, TEXT("레벨 로딩 완료"));
	}
	
	return bIsLevelLoaded;
}

bool UNS_LoadingScreen::CheckRenderingReady()
{
	if (bIsRenderingReady) return true;
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	// 게임 모드와 플레이어 컨트롤러가 준비되었는지 확인
	AGameModeBase* GameMode = World->GetAuthGameMode();
	APlayerController* PC = World->GetFirstPlayerController();
	
	if (GameMode && PC)
	{
		bIsRenderingReady = true;
		UE_LOG(LogTemp, Warning, TEXT("렌더링 준비 완료"));
	}
	
	return bIsRenderingReady;
}

bool UNS_LoadingScreen::CheckFrameRateStable()
{
	if (bIsFrameRateStable) return true;
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	// 현재 프레임률 계산
	float DeltaTime = World->GetDeltaSeconds();
	float CurrentFrameRate = (DeltaTime > 0.001f) ? (1.0f / DeltaTime) : 1000.0f;
	
	// 최근 프레임률 기록에 추가
	RecentFrameRates.Add(CurrentFrameRate);
	
	// 최대 60개의 프레임률 기록 유지 (약 1초간)
	if (RecentFrameRates.Num() > 60)
	{
		RecentFrameRates.RemoveAt(0);
	}
	
	// 충분한 샘플이 있을 때만 안정성 체크
	if (RecentFrameRates.Num() >= 30) // 최소 0.5초간의 데이터
	{
		// 평균 프레임률 계산
		float TotalFrameRate = 0.0f;
		for (float FrameRate : RecentFrameRates)
		{
			TotalFrameRate += FrameRate;
		}
		float AverageFrameRate = TotalFrameRate / RecentFrameRates.Num();
		
		// 안정적인 프레임률인지 확인
		if (AverageFrameRate >= MinRequiredFrameRate)
		{
			FrameRateCheckDuration += DeltaTime;
			
			// 충분한 시간 동안 안정적이었는지 확인
			if (FrameRateCheckDuration >= StableFrameCheckTime)
			{
				bIsFrameRateStable = true;
				UE_LOG(LogTemp, Warning, TEXT("프레임률 안정화 완료: 평균 %.1f FPS"), AverageFrameRate);
			}
		}
		else
		{
			// 프레임률이 떨어지면 다시 측정 시작
			FrameRateCheckDuration = 0.0f;
		}
	}
	
	return bIsFrameRateStable;
}

void UNS_LoadingScreen::OnLoadingFinished()
{
	if (!bIsLoading) return;
	
	bIsLoading = false;
	UE_LOG(LogTemp, Warning, TEXT("로딩 완료 - 로딩 스크린 제거"));
	
	// UIManager에게 로딩 완료 알림
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			if (UIManager->OnLoadingFinished.IsBound())
			{
				UIManager->OnLoadingFinished.Execute();
				UIManager->OnLoadingFinished.Unbind();
			}
		}
	}
	
	// 로딩 스크린 제거
	RemoveFromParent();
}
