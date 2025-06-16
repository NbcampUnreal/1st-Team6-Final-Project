// NS_ReadyUI.cpp
#include "NS_ReadyUI.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "NS_PlayerState.h"

// NS_ReadyUI.cpp
void UNS_ReadyUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddDynamic(this, &UNS_ReadyUI::OnReadyButtonClicked);
	}

	// ✅ 0.5초 후에 호출하도록 딜레이 설정
	FTimerHandle InitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		InitTimerHandle,
		this,
		&UNS_ReadyUI::UpdatePlayerStatusList,
		0.5f, // 딜레이
		false
	);
}

void UNS_ReadyUI::OnReadyButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
	
	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PC->PlayerState))
	{
		bool bNewReady = !PS->GetIsReady();

		PS->ServerSetIsReady(bNewReady);
	}
}


void UNS_ReadyUI::UpdatePlayerStatusList()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GameState = World->GetGameState();
	if (!GameState) return;

	const TArray<APlayerState*>& Players = GameState->PlayerArray;

	TArray<UTextBlock*> NameBlocks = { Text_Player0, Text_Player1, Text_Player2, Text_Player3 };
	TArray<UTextBlock*> StatusBlocks = { Text_Status0, Text_Status1, Text_Status2, Text_Status3 };

	for (int32 i = 0; i < NameBlocks.Num(); ++i)
	{
		if (!NameBlocks[i] || !StatusBlocks[i]) continue;

		if (i < Players.Num())
		{
			if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(Players[i]))
			{
				NameBlocks[i]->SetText(FText::FromString(PS->GetPlayerName()));
				StatusBlocks[i]->SetText(FText::FromString(PS->GetIsReady() ? TEXT("Ready") : TEXT("")));

				NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
				StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			// 인원 수 초과 시 숨김
			NameBlocks[i]->SetText(FText::FromString(TEXT("")));
			StatusBlocks[i]->SetText(FText::FromString(TEXT("")));

			NameBlocks[i]->SetVisibility(ESlateVisibility::Collapsed);
			StatusBlocks[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

