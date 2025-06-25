// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LoadingScreen.h"
#include "Components/ProgressBar.h"
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
	//ProgressBar_Loading->SetPercent(100.f);
	//if(1)return;

	TWeakObjectPtr<UNS_LoadingScreen> SafeThis = this;
	float ElapsedTime = 0.f;
	float PrevTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer
	(
		LoadingTickHandle,
		FTimerDelegate::CreateLambda([SafeThis, ElapsedTime, PrevTime]() mutable
		{
			if (!SafeThis.IsValid())return;

			UWorld* World = SafeThis->GetWorld();

			if (!World)return;

			ElapsedTime += World->GetTimeSeconds() - PrevTime;
			float Alpha = FMath::Clamp(ElapsedTime / 3.f, 0.f, 1.f);
			float CurValue = FMath::Lerp(0.f, 1.f, Alpha);
			UE_LOG(LogTemp, Warning, TEXT("ElapsedTime : %f / PrevTime :  %f / CurValue :  %f "), ElapsedTime, PrevTime, CurValue);
			SafeThis->ProgressBar_Loading->SetPercent(CurValue);

			if (1.f <= CurValue)
			{
				SafeThis->ProgressBar_Loading->SetPercent(1.f);
				if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()) )
				{
					if (GI->GetUIManager()->OnLoadingFinished.IsBound())
					{
						GI->GetUIManager()->OnLoadingFinished.Execute();
						GI->GetUIManager()->OnLoadingFinished.Unbind();
						UE_LOG(LogTemp, Warning, TEXT("OnLoadingFinished Execute ProgressBar_Loading 100 "));
					}
				}
			
				World->GetTimerManager().ClearTimer(SafeThis->LoadingTickHandle);
			}
			else
				PrevTime = World->GetTimeSeconds();

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