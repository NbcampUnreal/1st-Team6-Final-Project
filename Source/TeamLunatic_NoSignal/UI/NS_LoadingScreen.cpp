// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LoadingScreen.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/NS_UIManager.h"
#include "GameFlow/NS_GameInstance.h"

void UNS_LoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();

}
void UNS_LoadingScreen::FakeUpdateProgress()
{
	FakeProgressMax = 3.0f; // 15초 동안 로딩이 진행된다고 가정
	TWeakObjectPtr<UNS_LoadingScreen> SafeThis = this;
	float ElapsedTime = 0.f;
	float PrevTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer
	(
		LoadingTickHandle,
		FTimerDelegate::CreateLambda([SafeThis, ElapsedTime, PrevTime]() mutable
			{
				if (!SafeThis.IsValid())return;

				UWorld* World = GEngine->GetWorldFromContextObjectChecked(SafeThis.Get());

				if (!World)return;

				ElapsedTime += World->GetTimeSeconds() - PrevTime;
				float Alpha = FMath::Clamp(ElapsedTime / SafeThis->FakeProgressMax, 0.f, 1.f);
				float CurValue = FMath::Lerp(0.f, 1.f, Alpha);
				//UE_LOG(LogTemp, Warning, TEXT("ElapsedTime : %f / PrevTime :  %f / TimeSeconds :  %f "), FEmt.ElapsedTime, FEmt.PrevTime, GetWorld()->GetTimeSeconds());

				if (SafeThis->bIsLevelLoadComplete&& SafeThis->FakeProgressMax != 5.f)
				{
					SafeThis->FakeProgressMax = 5.f;
					ElapsedTime = 3.5f;
				}
				else
				{
					if (0.6f < CurValue)
						CurValue = 0.6f;
				}

				SafeThis->ProgressBar_Loading->SetPercent(CurValue);

				if (1.f <= CurValue)
				{
					SafeThis->ProgressBar_Loading->SetPercent(CurValue);
					if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()))
					{
						if (GI->GetUIManager()->OnLoadingFinished.IsBound())
						{
							GI->GetUIManager()->OnLoadingFinished.Execute();
							GI->GetUIManager()->OnLoadingFinished.Unbind();
						}
					}

					World->GetTimerManager().ClearTimer(SafeThis->LoadingTickHandle);
				}
				else
					PrevTime = World->GetTimeSeconds();

			}), 0.05f, true
	);
}

void UNS_LoadingScreen::UpdateProgress()
{
	// 실제 게임 상태를 체크하는 로딩 시스템
	TWeakObjectPtr<UNS_LoadingScreen> SafeThis = this;
	float ElapsedTime = 0.f;
	float PrevTime = GetWorld()->GetTimeSeconds();

	// 초기화
	bIsRenderingReady = false;
	bIsFrameRateStable = false;
	FrameRateCheckDuration = 0.f;
	RecentFrameRates.Empty();

#if WITH_EDITOR
	// 에디터에서는 더 빠른 체크를 위해 기준 완화
	MinRequiredFrameRate = 30.0f;
	StableFrameCheckTime = 0.5f;
#else
	// 패키징 빌드에서는 더 엄격한 기준
	MinRequiredFrameRate = 45.0f;
	StableFrameCheckTime = 1.0f;
#endif

	GetWorld()->GetTimerManager().SetTimer(
		LoadingTickHandle,
		FTimerDelegate::CreateLambda([SafeThis, ElapsedTime, PrevTime]() mutable
		{
			if (!SafeThis.IsValid()) return;

			UWorld* World = SafeThis->GetWorld();
			if (!World) return;

			if (SafeThis->ProgressBar_Loading->GetPercent() < 1.f)
			{
				ElapsedTime += World->GetTimeSeconds() - PrevTime;

				// 실제 게임 준비 상태 업데이트
				SafeThis->UpdateRealLoadingProgress();

				float CurValue = 0.f;

				// 실제 인게임 상태 기반 진행률 계산
				if (!SafeThis->bIsLevelLoadComplete)
				{
					// 1단계: 레벨 로딩 (0% ~ 30%) - 빠르게 완료
					float Alpha = FMath::Clamp(ElapsedTime / 1.0f, 0.f, 1.f);
					CurValue = FMath::Lerp(0.f, 0.3f, Alpha);

					// 1초 후에는 레벨 로딩 완료로 간주 (이미 인게임에 있으므로)
					if (ElapsedTime >= 1.0f)
					{
						SafeThis->bIsLevelLoadComplete = true;
					}
				}
				else if (!SafeThis->bIsRenderingReady)
				{
					// 2단계: 인게임 렌더링 준비 (30% ~ 60%)
					if (SafeThis->CheckRenderingReadiness())
					{
						CurValue = 0.6f;
					}
					else
					{
						// 렌더링 준비 진행 중
						float RenderProgress = FMath::Clamp((ElapsedTime - 1.0f) / 2.0f, 0.f, 1.f);
						CurValue = 0.3f + (RenderProgress * 0.3f);
					}
				}
				else if (!SafeThis->bIsFrameRateStable)
				{
					// 3단계: 실제 인게임 프레임률 안정화 (60% ~ 100%)
					float StabilityProgress = SafeThis->FrameRateCheckDuration / SafeThis->StableFrameCheckTime;
					CurValue = 0.6f + (StabilityProgress * 0.4f);

					// 프레임률이 안정되면 100% 완료
					if (SafeThis->bIsFrameRateStable)
					{
						CurValue = 1.0f;
					}
				}
				else
				{
					// 모든 준비 완료 - 인게임에서 45fps 이상 달성
					CurValue = 1.0f;
				}

				SafeThis->ProgressBar_Loading->SetPercent(CurValue);

				// 퍼센트 텍스트 업데이트
				if (SafeThis->Text_LoadingPercent)
				{
					FString PercentText = FString::Printf(TEXT("%.0f%%"), CurValue * 100.0f);
					SafeThis->Text_LoadingPercent->SetText(FText::FromString(PercentText));
				}

				// 로딩 상태 텍스트 업데이트 (인게임 상황에 맞게)
				if (SafeThis->Text_LoadingStatus)
				{
					FString StatusText;
					if (!SafeThis->bIsLevelLoadComplete)
					{
						StatusText = TEXT("게임 초기화 중...");
					}
					else if (!SafeThis->bIsRenderingReady)
					{
						StatusText = TEXT("인게임 렌더링 최적화 중...");
					}
					else if (!SafeThis->bIsFrameRateStable)
					{
						// 현재 프레임률 표시
						float CurrentFPS = SafeThis->RecentFrameRates.Num() > 0 ?
							SafeThis->RecentFrameRates.Last() : 0.0f;
						StatusText = FString::Printf(TEXT("프레임률 안정화 중... (현재: %.0f FPS, 목표: %.0f FPS)"),
							CurrentFPS, SafeThis->MinRequiredFrameRate);
					}
					else
					{
						StatusText = FString::Printf(TEXT("게임 준비 완료! (%.0f FPS 달성)"),
							SafeThis->MinRequiredFrameRate);
					}
					SafeThis->Text_LoadingStatus->SetText(FText::FromString(StatusText));
				}

				UE_LOG(LogTemp, Log, TEXT("로딩 진행률: %.1f%% | 레벨완료: %s | 렌더링준비: %s | 프레임안정: %s"),
					CurValue * 100.0f,
					SafeThis->bIsLevelLoadComplete ? TEXT("O") : TEXT("X"),
					SafeThis->bIsRenderingReady ? TEXT("O") : TEXT("X"),
					SafeThis->bIsFrameRateStable ? TEXT("O") : TEXT("X"));

				if (CurValue >= 1.0f && SafeThis->bIsFrameRateStable)
				{
					// 100% 완료 및 모든 준비가 완료되면 로딩 종료
					SafeThis->ProgressBar_Loading->SetPercent(1.f);

					// 최종 텍스트 업데이트
					if (SafeThis->Text_LoadingPercent)
					{
						SafeThis->Text_LoadingPercent->SetText(FText::FromString(TEXT("100%")));
					}
					if (SafeThis->Text_LoadingStatus)
					{
						SafeThis->Text_LoadingStatus->SetText(FText::FromString(TEXT("게임 시작!")));
					}

					if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()))
					{
						// UIManager를 통해 로딩 스크린 숨기기
						if (UNS_UIManager* UIManager = GI->GetUIManager())
						{
							UIManager->HideLoadingScreen(World);
						}

						// 기존 델리게이트도 실행
						if (GI->GetUIManager()->OnLoadingFinished.IsBound())
						{
							GI->GetUIManager()->OnLoadingFinished.Execute();
							GI->GetUIManager()->OnLoadingFinished.Unbind();
						}

						UE_LOG(LogTemp, Warning, TEXT("100%% 완료 - 인게임 45fps 이상 달성 - 로딩 스크린 제거"));
					}
					World->GetTimerManager().ClearTimer(SafeThis->LoadingTickHandle);
				}
				else
				{
					PrevTime = World->GetTimeSeconds();
				}
			}
		}), 0.05f, true
	);
}

void UNS_LoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	//{
	//	//float Progress = GI->GetLoadingProgress();
	//	ProgressBar_Loading->SetPercent(InDeltaTime*0.01f);
	//}
}

bool UNS_LoadingScreen::CheckRenderingReadiness()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// 월드가 완전히 로드되었는지 확인
	if (World->AreActorsInitialized() && World->HasBegunPlay())
	{
		bIsRenderingReady = true;
		return true;
	}

	return false;
}

bool UNS_LoadingScreen::CheckFrameRateStability()
{
	if (!bIsRenderingReady) return false;

	// 현재 프레임률 계산 (DeltaTime이 0에 가까우면 매우 높은 프레임률로 처리)
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float CurrentFrameRate = (DeltaTime > 0.001f) ? (1.0f / DeltaTime) : 1000.0f;

	// 최근 프레임률 기록에 추가
	RecentFrameRates.Add(CurrentFrameRate);

	// 최대 60개의 프레임률 기록 유지 (약 3초간)
	if (RecentFrameRates.Num() > 60)
	{
		RecentFrameRates.RemoveAt(0);
	}

	// 충분한 데이터가 쌓였는지 확인
	if (RecentFrameRates.Num() >= 30) // 최소 1.5초간의 데이터
	{
		// 평균 프레임률 계산
		float AverageFrameRate = 0.0f;
		for (float FrameRate : RecentFrameRates)
		{
			AverageFrameRate += FrameRate;
		}
		AverageFrameRate /= RecentFrameRates.Num();

		// 프레임률이 안정적인지 확인
		if (AverageFrameRate >= MinRequiredFrameRate)
		{
			FrameRateCheckDuration += GetWorld()->GetDeltaSeconds();

			// 충분한 시간 동안 안정적이었는지 확인
			if (FrameRateCheckDuration >= StableFrameCheckTime)
			{
				bIsFrameRateStable = true;
				UE_LOG(LogTemp, Log, TEXT("프레임률 안정화 완료: 평균 %.1f FPS"), AverageFrameRate);
				return true;
			}
		}
		else
		{
			// 프레임률이 떨어지면 다시 측정 시작
			FrameRateCheckDuration = 0.0f;
			UE_LOG(LogTemp, Warning, TEXT("프레임률 불안정: 현재 %.1f FPS (요구: %.1f FPS)"), AverageFrameRate, MinRequiredFrameRate);
		}
	}

	return false;
}

void UNS_LoadingScreen::UpdateRealLoadingProgress()
{
	// 렌더링 준비 상태 체크
	if (!bIsRenderingReady)
	{
		CheckRenderingReadiness();
	}

	// 프레임률 안정성 체크
	if (bIsRenderingReady && !bIsFrameRateStable)
	{
		CheckFrameRateStability();
	}
}