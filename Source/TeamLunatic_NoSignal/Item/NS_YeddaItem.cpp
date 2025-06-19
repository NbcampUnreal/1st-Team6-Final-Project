// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/NS_YeddaItem.h"
#include "GameFlow/NS_GameInstance.h"
#include "UI/NS_UIManager.h"
#include "UI/NS_PlayerHUD.h"

ANS_YeddaItem::ANS_YeddaItem()
{

}

void ANS_YeddaItem::BeginPlay()
{
	Super::BeginPlay();

	// 0.2초 후에 HUD에 전달
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(
		DelayHandle,
		this,
		&ANS_YeddaItem::TryAssignToHUD,
		0.2f,
		false
	);
}
void ANS_YeddaItem::TryAssignToHUD()
{
	if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
	{
		if (UNS_UIManager* UIManager = GI->GetUIManager())
		{
			if (UNS_PlayerHUD* PlayerHUD = UIManager->GetPlayerHUDWidget())
			{
				PlayerHUD->SetYeddaItem(this);
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ANS_YeddaItem::TryAssignToHUD "));

	// HUD가 아직 생성되지 않았을 경우 재시도
	FTimerHandle RetryHandle;
	GetWorldTimerManager().SetTimer(
		RetryHandle,
		this,
		&ANS_YeddaItem::TryAssignToHUD,
		0.2f,
		false
	);
}