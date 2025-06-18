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

	// 0.5초 후에 호출하도록 딜레이 설정
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

	TArray<APlayerState*> SortedPlayers = GameState->PlayerArray;

	// PlayerIndex 기준 정렬
	SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const ANS_PlayerState* PSA = Cast<const ANS_PlayerState>(&A);
		const ANS_PlayerState* PSB = Cast<const ANS_PlayerState>(&B);
		return (PSA && PSB) ? PSA->PlayerIndex < PSB->PlayerIndex : false;
	});

	TArray<UTextBlock*> NameBlocks = { Text_Player0, Text_Player1, Text_Player2, Text_Player3 };
	TArray<UTextBlock*> StatusBlocks = { Text_Status0, Text_Status1, Text_Status2, Text_Status3 };
	TArray<UImage*> ArrowImages = { image_0, image_1, image_2, image_3 };

	// 내 PlayerIndex 구하기
	int32 MyIndex = -1;
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ANS_PlayerState* MyPS = Cast<ANS_PlayerState>(PC->PlayerState))
		{
			MyIndex = MyPS->PlayerIndex;
		}
	}

	for (int32 i = 0; i < NameBlocks.Num(); ++i)
	{
		if (!NameBlocks[i] || !StatusBlocks[i] || !ArrowImages[i]) continue;

		if (i < SortedPlayers.Num())
		{
			if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(SortedPlayers[i]))
			{
				NameBlocks[i]->SetText(FText::FromString(FString::Printf(TEXT("Player %d"), PS->PlayerIndex + 1)));
				StatusBlocks[i]->SetText(FText::FromString(PS->GetIsReady() ? TEXT("Ready") : TEXT("")));

				NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
				StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);

				// 자기 자신의 인덱스라면 화살표 표시
				if (PS->PlayerIndex == MyIndex)
				{
					ArrowImages[i]->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ArrowImages[i]->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
		else
		{
			NameBlocks[i]->SetText(FText::GetEmpty());
			StatusBlocks[i]->SetText(FText::GetEmpty());

			NameBlocks[i]->SetVisibility(ESlateVisibility::Collapsed);
			StatusBlocks[i]->SetVisibility(ESlateVisibility::Collapsed);
			ArrowImages[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

