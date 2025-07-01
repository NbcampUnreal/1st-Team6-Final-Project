// Fill out your copyright notice in the Description page of Project Settings.


#include "World/EndingUI/NS_EndingStatusUI.h"

void UNS_EndingStatusUI::EndingUpdateStatus(int32 NumPlayersInZone, int32 TotalPlayerCount, int32 NumItemsCollected)
{
	if (PlayerCountText)
	{
		PlayerCountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumPlayersInZone, TotalPlayerCount)));
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::FromString(FString::Printf(TEXT("%d / 2"), NumItemsCollected)));
	}
}

void UNS_EndingStatusUI::UpdateRemainingTime(float TimeRemaining)
{
	if (!TimerText) return;

	if (TimeRemaining <= 0.f)
	{
		// 텍스트 숨김
		TimerText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 텍스트 갱신 및 표시
	int32 Seconds = FMath::CeilToInt(TimeRemaining);
	FString TimeString = FString::Printf(TEXT("탈출까지 %d초..."), Seconds);
	TimerText->SetText(FText::FromString(TimeString));

	// 필요 시 다시 보이게 설정
	TimerText->SetVisibility(ESlateVisibility::Visible);
}
