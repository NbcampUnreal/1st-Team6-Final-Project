// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LogoScreenWidget.h"


void UNS_LogoScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ShowLogo)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &UNS_LogoScreenWidget::OnShowLogoFinished);

		BindToAnimationFinished(ShowLogo, FinishedEvent);

		PlayAnimation(ShowLogo);
	}
}
void UNS_LogoScreenWidget::OnShowLogoFinished()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]() mutable
		{
			
			// UE_LOG(LogTemp, Warning, TEXT("Clear!!!!!! TimerHandle/ Index : %d"), Index);
		}), 1.6f, false
	);
}