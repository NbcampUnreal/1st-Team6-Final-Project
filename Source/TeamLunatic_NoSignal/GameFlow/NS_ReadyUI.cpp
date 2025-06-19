// NS_ReadyUI.cpp
#include "NS_ReadyUI.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "NS_GameInstance.h"
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

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UNS_ReadyUI::OnQuitButtonClicked);
	}

}

void UNS_ReadyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdatePlayerStatusList(); // 실시간 갱신
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

void UNS_ReadyUI::OnQuitButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	if (ANS_PlayerState* PS = Cast<ANS_PlayerState>(PC->PlayerState))
	{
		int32 MyIndex = PS->PlayerIndex;

		if (UNS_GameInstance* GI = Cast<UNS_GameInstance>(GetGameInstance()))
		{
			if (MyIndex == 0)
			{
				GI->DestroyCurrentSession();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ReadyUI] Guest가 세션에서 이탈합니다 (Index: %d)"), MyIndex);
			}
		}

		PC->ClientTravel(TEXT("/Game/Maps/MainTitle"), ETravelType::TRAVEL_Absolute);
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
				StatusBlocks[i]->SetText(FText::FromString(PS->GetIsReady() ? TEXT("Ready") : TEXT("Not Ready")));

				// 항상 보이게
				NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
				StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);

				ArrowImages[i]->SetVisibility(PS->PlayerIndex == MyIndex ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			}
		}
		else
		{
			// 빈 슬롯도 표기
			NameBlocks[i]->SetText(FText::FromString(FString::Printf(TEXT("Empty Slot %d"), i + 1)));
			StatusBlocks[i]->SetText(FText::FromString(TEXT("-")));

			NameBlocks[i]->SetVisibility(ESlateVisibility::Visible);
			StatusBlocks[i]->SetVisibility(ESlateVisibility::Visible);
			ArrowImages[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

}

