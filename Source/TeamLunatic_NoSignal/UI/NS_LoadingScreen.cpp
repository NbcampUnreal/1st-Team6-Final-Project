// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LoadingScreen.h"
#include "Components/ProgressBar.h"
#include "UI/NS_UIManager.h"
#include "GameFlow/NS_GameInstance.h"

void UNS_LoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();

}
void UNS_LoadingScreen::UpdateProgress()
{
	bIsFinished = false;

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
			float Alpha = FMath::Clamp(ElapsedTime / 10.f, 0.f, 1.f);
			float CurValue = FMath::Lerp(0.f, 1.f, Alpha);
			//UE_LOG(LogTemp, Warning, TEXT("ElapsedTime : %f / PrevTime :  %f / TimeSeconds :  %f "), FEmt.ElapsedTime, FEmt.PrevTime, GetWorld()->GetTimeSeconds());
			SafeThis->ProgressBar_Loading->SetPercent(CurValue);

			if (1.f <= Alpha)
			{
				SafeThis->ProgressBar_Loading->SetPercent(CurValue);
				SafeThis->bIsFinished = true;
				if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(World->GetGameInstance()) )
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

