// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_LogoScreenWidget.h"


void UNS_LogoScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 첫 번째 로고 애니메이션 설정
	if (ShowLogo)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &UNS_LogoScreenWidget::OnShowLogoFinished);
		BindToAnimationFinished(ShowLogo, FinishedEvent);
		
		// 첫 번째 로고 애니메이션 재생
		PlayAnimation(ShowLogo);
	}
	else
	{
		// 첫 번째 로고 애니메이션이 없으면 바로 두 번째 로고 애니메이션 재생
		StartSecondLogoAnimation();
	}
	
	// 두 번째 로고 애니메이션 설정
	if (ShowSecondLogo)
	{
		FWidgetAnimationDynamicEvent SecondFinishedEvent;
		SecondFinishedEvent.BindDynamic(this, &UNS_LogoScreenWidget::OnShowSecondLogoFinished);
		BindToAnimationFinished(ShowSecondLogo, SecondFinishedEvent);
	}
}

void UNS_LogoScreenWidget::OnShowLogoFinished()
{
	// 첫 번째 로고 애니메이션이 끝나면 잠시 대기 후 두 번째 로고 애니메이션 시작
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]() mutable
		{
			StartSecondLogoAnimation();
		}), 0.5f, false  // 0.5초 대기 후 두 번째 로고 시작
	);
}

void UNS_LogoScreenWidget::StartSecondLogoAnimation()
{
	// 두 번째 로고 애니메이션 재생
	if (ShowSecondLogo)
	{
		PlayAnimation(ShowSecondLogo);
	}
	else
	{
		// 두 번째 로고 애니메이션이 없으면 바로 완료 처리
		OnShowSecondLogoFinished();
	}
}

void UNS_LogoScreenWidget::OnShowSecondLogoFinished()
{
	// 두 번째 로고 애니메이션이 끝나면 잠시 대기 후 메뉴로 전환
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this]() mutable
		{
			// 모든 로고 애니메이션이 끝났음을 알림
			OnLogosFinished.Broadcast();
			
			// 필요한 경우 여기서 메뉴 위젯을 직접 호출할 수도 있음
			// 예: UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
		}), 1.0f, false  // 1초 대기 후 메뉴로 전환
	);
}
